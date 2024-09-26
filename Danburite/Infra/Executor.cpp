#include "Executor.h"

namespace Infra
{
	Executor::Executor()
	{
		__thread = std::thread{ &Executor::__loop, this };
	}

	Executor::~Executor() noexcept
	{
		__running = false;
		__thread.join();
	}

	void Executor::waitIdle()
	{
		run([] { }).wait();
	}

	std::future<void> Executor::run(
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
		}

		return retVal;
	}

	void Executor::silentRun(
		Job &&job)
	{
		__JobInfo jobInfo
		{
			.job{ std::move(job) }
		};

		{
			std::lock_guard loopLock{ __loopMutex };
			__jobInfos.emplace_back(std::move(jobInfo));
		}
	}

	void Executor::__loop()
	{
		std::unique_lock loopLock{ __loopMutex, std::defer_lock };
		std::vector<__JobInfo> inFlightJobInfos;

		while (__running)
		{
			loopLock.lock();
			inFlightJobInfos.swap(__jobInfos);
			loopLock.unlock();

			for (auto &jobInfo : inFlightJobInfos)
			{
				jobInfo.job();
				jobInfo.signal();
			}

			inFlightJobInfos.clear();
			__idleEvent.invoke(this);
		}
	}

	void Executor::__JobInfo::signal() noexcept
	{
		if (!(optPromise.has_value()))
			return;

		auto &promise{ optPromise.value() };
		promise.set_value();
	}
}