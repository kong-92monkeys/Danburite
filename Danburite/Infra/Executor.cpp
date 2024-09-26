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

		{
			std::lock_guard loopLock{ __loopMutex };
			__loopCV.notify_all();
		}

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
			__loopCV.notify_all();
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
			__loopCV.notify_all();
		}
	}

	void Executor::__loop()
	{
		std::unique_lock loopLock{ __loopMutex, std::defer_lock };
		std::vector<__JobInfo> inFlightJobInfos;

		while (true)
		{
			auto const beginningTime{ std::chrono::steady_clock::now() };

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

			auto const delay{ (beginningTime + __loopInterval) - std::chrono::steady_clock::now() };
			std::this_thread::sleep_for(delay);

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