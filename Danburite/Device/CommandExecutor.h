#pragma once

#include "../Vulkan/CommandBuffer.h"
#include <vector>
#include <functional>

namespace Dev
{
	class CommandExecutor : public Infra::Unique
	{
	public:
		using Job = std::function<void(VK::CommandBuffer &)>;

		void reserve(
			Job &&job) noexcept;

		void execute(
			VK::CommandBuffer &cmdBuffer) noexcept;

	private:
		std::vector<Job> __jobs;
	};
}