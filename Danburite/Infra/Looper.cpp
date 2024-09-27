#include "Looper.h"

namespace Infra
{
	Looper::Looper()
	{
		__thread = std::thread{ &Looper::__loop, this };
	}

	Looper::~Looper() noexcept
	{
		__running = false;
		__thread.join();
	}

	void Looper::waitIdle()
	{
		run([] { }).wait();
	}

	std::future<void> Looper::run(
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

	void Looper::silentRun(
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

	void Looper::__loop()
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

	void Looper::__JobInfo::signal() noexcept
	{
		if (!(optPromise.has_value()))
			return;

		auto &promise{ optPromise.value() };
		promise.set_value();
	}
}