#include "CommandExecutor.h"

namespace Dev
{
	void CommandExecutor::reserve(
		Job &&job) noexcept
	{
		__jobs.emplace_back(std::move(job));
	}

	void CommandExecutor::execute(
		VK::CommandBuffer &cmdBuffer)
	{
		for (auto const &job : __jobs)
			job(cmdBuffer);

		__jobs.clear();
	}
}