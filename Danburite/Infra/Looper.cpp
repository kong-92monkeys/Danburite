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

		{
			std::lock_guard loopLock{ __loopMutex };
			__jobInfos.emplace_back(std::move(job), std::move(promise));
		}

		return retVal;
	}

	void Looper::silentRun(
		Job &&job)
	{
		std::lock_guard loopLock{ __loopMutex };
		__jobInfos.emplace_back(std::move(job), std::nullopt);
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
				jobInfo.run();

			inFlightJobInfos.clear();
			__idleEvent.invoke(this);
		}
	}

	Looper::__JobInfo::__JobInfo(
		Job &&job,
		std::optional<std::promise<void>> optPromise) noexcept :
		__job			{ std::move(job) },
		__optPromise	{ std::move(optPromise) }
	{}

	void Looper::__JobInfo::run()
	{
		__job();

		if (__optPromise.has_value())
			__optPromise.value().set_value();
	}
}