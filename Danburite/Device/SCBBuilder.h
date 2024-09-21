#pragma once

#include "../Infra/ThreadPool.h"
#include "CommandBufferCirculator.h"

namespace Dev
{
	class SCBBuilder : public Infra::Unique
	{
	public:
		SCBBuilder(
			VK::Device &device,
			uint32_t queueFamilyIndex);

		[[nodiscard]]
		std::future<VK::CommandBuffer *> build(
			std::function<void(VK::CommandBuffer &)> &&logic);

		[[nodiscard]]
		constexpr size_t getCapacity() const noexcept;

	private:
		Infra::ThreadPool __threadPool;

		std::unique_ptr<CommandBufferCirculator> __pCirculator;
	};

	constexpr size_t SCBBuilder::getCapacity() const noexcept
	{
		return __threadPool.getPoolSize();
	}
}