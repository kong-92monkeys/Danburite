#include "ThreadPool.h"

namespace Infra
{
	ThreadPool::ThreadPool(
		size_t const poolSize)
	{
		__slotInfos.resize(poolSize);
		__threadIds.resize(poolSize);

		for (size_t slotIt{ }; slotIt < poolSize; ++slotIt)
		{
			auto &pSlotInfo{ __slotInfos[slotIt] };
			pSlotInfo = std::make_unique<__SlotInfo>();

			auto &thread{ pSlotInfo->thread };
			thread = std::thread{ &ThreadPool::__loop, this, slotIt };

			__threadIds[slotIt] = thread.get_id();
		}
	}

	ThreadPool::~ThreadPool() noexcept
	{
		__running = false;

		for (auto const &pSlotInfo : __slotInfos)
		{
			std::lock_guard loopLock	{ pSlotInfo->loopMutex };
			auto &loopCV				{ pSlotInfo->loopCV };
			loopCV.notify_all();
		}

		for (auto const &pSlotInfo : __slotInfos)
			pSlotInfo->thread.join();
	}

	void ThreadPool::waitIdle(
		size_t const threadIndex)
	{
		run(threadIndex, [] { }).wait();
	}

	void ThreadPool::waitIdle()
	{
		size_t const poolSize{ getPoolSize() };
		for (size_t slotIter{ }; slotIter < poolSize; ++slotIter)
			run(slotIter, [] { }).wait();
	}

	std::future<void> ThreadPool::run(
		size_t const threadIndex,
		Job &&job)
	{
		auto const &pSlotInfo	{ __slotInfos[threadIndex] };
		auto &loopCV			{ pSlotInfo->loopCV };
		auto &jobInfos			{ pSlotInfo->jobInfos };

		std::promise<void> promise;
		std::future<void> retVal{ promise.get_future() };

		__JobInfo jobInfo
		{
			.job		{ std::move(job) },
			.promise	{ std::move(promise) }
		};

		{
			std::lock_guard loopLock{ pSlotInfo->loopMutex };
			jobInfos.emplace_back(std::move(jobInfo));
			loopCV.notify_all();
		}

		return retVal;
	}

	std::future<void> ThreadPool::run(
		Job &&job)
	{
		__randomSlotIndex = ((__randomSlotIndex + 1ULL) % getPoolSize());
		return run(__randomSlotIndex, std::move(job));
	}

	void ThreadPool::silentRun(
		size_t const threadIndex,
		Job &&job)
	{
		auto const &pSlotInfo	{ __slotInfos[threadIndex] };
		auto &loopCV			{ pSlotInfo->loopCV };
		auto &jobInfos			{ pSlotInfo->jobInfos };

		__JobInfo jobInfo
		{
			.job{ std::move(job) }
		};

		{
			std::lock_guard loopLock{ pSlotInfo->loopMutex };
			jobInfos.emplace_back(std::move(jobInfo));
			loopCV.notify_all();
		}
	}

	void ThreadPool::silentRun(
		Job &&job)
	{
		__randomSlotIndex = ((__randomSlotIndex + 1ULL) % getPoolSize());
		silentRun(__randomSlotIndex, std::move(job));
	}

	void ThreadPool::__loop(
		size_t const threadIndex)
	{
		auto const &pSlotInfo		{ __slotInfos[threadIndex] };

		std::unique_lock loopLock	{ pSlotInfo->loopMutex, std::defer_lock };
		auto &loopCV				{ pSlotInfo->loopCV };
		auto &jobInfos				{ pSlotInfo->jobInfos };

		std::vector<__JobInfo> inFlightJobInfos;

		while (true)
		{
			loopLock.lock();

			loopCV.wait(loopLock, [this, &jobInfos]
			{
				return (!__running || jobInfos.size());
			});

			if (!__running)
				break;

			inFlightJobInfos.swap(jobInfos);
			loopLock.unlock();

			for (auto &jobInfo : inFlightJobInfos)
			{
				jobInfo.job();
				jobInfo.signal();
			}

			inFlightJobInfos.clear();
		}
	}

	void ThreadPool::__JobInfo::signal() noexcept
	{
		if (promise.has_value())
			promise.value().set_value();
	}
}