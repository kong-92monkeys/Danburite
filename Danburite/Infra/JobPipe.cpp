#include "JobPipe.h"

namespace Infra
{
	void JobPipe::waitIdle()
	{
		run([] { }).wait();
	}

	std::future<void> JobPipe::run(
		Job &&job)
	{
		std::promise<void> promise;
		std::future<void> retVal{ promise.get_future() };

		{
			std::lock_guard lock{ __mutex };
			__jobInfos.emplace_back(std::move(job), std::move(promise));
		}

		return retVal;
	}

	void JobPipe::silentRun(
		Job &&job)
	{
		std::lock_guard lock{ __mutex };
		__jobInfos.emplace_back(std::move(job), std::nullopt);
	}

	void JobPipe::receive(
		std::vector<JobInfo> &jobs)
	{
		std::lock_guard lock{ __mutex };
		jobs.swap(__jobInfos);
	}

	JobPipe::JobInfo::JobInfo(
		Job &&job,
		std::optional<std::promise<void>> optPromise) noexcept :
		__job			{ std::move(job) },
		__optPromise	{ std::move(optPromise) }
	{}

	void JobPipe::JobInfo::run()
	{
		__job();

		if (__optPromise.has_value())
			__optPromise.value().set_value();
	}
}