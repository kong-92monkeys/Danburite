#pragma once

#include "../Infra/ThreadPool.h"
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

		virtual ~CommandExecutor() noexcept override;
		
		void reserve(
			Job &&job) noexcept;

		[[nodiscard]]
		constexpr bool isEmpty() const noexcept;

		[[nodiscard]]
		std::future<VK::CommandBuffer *> execute() noexcept;

	private:
		Infra::ThreadPool __executionThread{ 1ULL };

		std::unique_ptr<CommandBufferCirculator> __pCmdBufferCirculator;
		std::vector<Job> __jobs;
	};

	constexpr bool CommandExecutor::isEmpty() const noexcept
	{
		return __jobs.empty();
	}
}