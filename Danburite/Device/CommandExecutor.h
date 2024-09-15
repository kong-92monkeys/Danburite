#pragma once

#include "../Vulkan/CommandBuffer.h"
#include "CommandBufferCirculator.h"
#include <memory>
#include <vector>
#include <functional>

namespace Dev
{
	class CommandExecutor : public Infra::Unique
	{
	public:
		using Job = std::function<void(VK::CommandBuffer &)>;

		CommandExecutor(
			VK::Device &device,
			uint32_t queueFamilyIndex);

		virtual ~CommandExecutor() noexcept override = default;
		
		void reserve(
			Job &&job) noexcept;

		[[nodiscard]]
		VK::CommandBuffer *execute() noexcept;

	private:
		std::unique_ptr<CommandBufferCirculator> __pCmdBufferCirculator;
		std::vector<Job> __jobs;
	};
}