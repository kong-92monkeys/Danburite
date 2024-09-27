#include "SingleThreadPool.h"

namespace Infra
{
	SingleThreadPool::SingleThreadPool()
	{
		__thread = std::thread{ &SingleThreadPool::__loop, this };
	}

	SingleThreadPool::~SingleThreadPool() noexcept
	{
		__running = false;

		{
			std::lock_guard loopLock{ __loopMutex };
			__loopCV.notify_all();
		}

		__thread.join();
	}

	void SingleThreadPool::waitIdle()
	{
		run([] { }).wait();
	}

	std::future<void> SingleThreadPool::run(
		Job &&job)
	{
		std::promise<void> promise;
		std::future<void> retVal{ promise.get_future() };

		__JobInfo jobInfo
		{
			.job		{ std::move(job) },
			.optPromise	{ std::move(promise) }
		};

		{
			std::lock_guard loopLock{ __loopMutex };
			__jobInfos.emplace_back(std::move(jobInfo));
			__loopCV.notify_all();
		}

		return retVal;
	}

	void SingleThreadPool::silentRun(
		Job &&job)
	{
		__JobInfo jobInfo
		{
			.job{ std::move(job) }
		};

		{
			std::lock_guard loopLock{ __loopMutex };
			__jobInfos.emplace_back(std::move(jobInfo));
			__loopCV.notify_all();
		}
	}

	void SingleThreadPool::__loop()
	{
		std::unique_lock loopLock{ __loopMutex, std::defer_lock };
		std::vector<__JobInfo> inFlightJobInfos;

		while (true)
		{
			loopLock.lock();

			__loopCV.wait(loopLock, [this]
			{
				return (!__running || __jobInfos.size());
			});

			if (!__running)
				break;

			inFlightJobInfos.swap(__jobInfos);
			loopLock.unlock();

			for (auto &jobInfo : inFlightJobInfos)
			{
				jobInfo.job();
				jobInfo.signal();
			}

			inFlightJobInfos.clear();
		}
	}

	void SingleThreadPool::__JobInfo::signal() noexcept
	{
		if (!(optPromise.has_value()))
			return;

		auto &promise{ optPromise.value() };
		promise.set_value();
	}
}