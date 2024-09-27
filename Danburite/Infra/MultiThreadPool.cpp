#include "MultiThreadPool.h"

namespace Infra
{
	MultiThreadPool::MultiThreadPool(
		size_t const poolSize)
	{
		__slotInfos.resize(poolSize);
		__threadIds.resize(poolSize);

		for (size_t slotIt{ }; slotIt < poolSize; ++slotIt)
		{
			auto &pSlotInfo{ __slotInfos[slotIt] };
			pSlotInfo = std::make_unique<__SlotInfo>();

			auto &thread{ pSlotInfo->thread };
			thread = std::thread{ &MultiThreadPool::__loop, this, slotIt };

			__threadIds[slotIt] = thread.get_id();
		}
	}

	MultiThreadPool::~MultiThreadPool() noexcept
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

	void MultiThreadPool::waitIdle()
	{
		size_t const poolSize{ getPoolSize() };
		for (size_t slotIter{ }; slotIter < poolSize; ++slotIter)
			run(slotIter, [] { }).wait();
	}

	std::future<void> MultiThreadPool::run(
		Job &&job)
	{
		__randomSlotIndex = ((__randomSlotIndex + 1ULL) % getPoolSize());
		return run(__randomSlotIndex, std::move(job));
	}

	void MultiThreadPool::silentRun(
		Job &&job)
	{
		__randomSlotIndex = ((__randomSlotIndex + 1ULL) % getPoolSize());
		silentRun(__randomSlotIndex, std::move(job));
	}

	void MultiThreadPool::waitIdle(
		size_t const threadIndex)
	{
		run(threadIndex, [] { }).wait();
	}

	std::future<void> MultiThreadPool::run(
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
			.optPromise	{ std::move(promise) }
		};

		{
			std::lock_guard loopLock{ pSlotInfo->loopMutex };
			jobInfos.emplace_back(std::move(jobInfo));
			loopCV.notify_all();
		}

		return retVal;
	}

	void MultiThreadPool::silentRun(
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

	void MultiThreadPool::__loop(
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

	void MultiThreadPool::__JobInfo::signal() noexcept
	{
		if (!(optPromise.has_value()))
			return;

		auto &promise{ optPromise.value() };
		promise.set_value();
	}
}