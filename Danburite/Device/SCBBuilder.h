#pragma once

#include "../Infra/MultiThreadPool.h"
#include "CommandBufferCirculator.h"

namespace Dev
{
	class SCBBuilder : public Infra::Unique
	{
	public:
		SCBBuilder(
			VK::Device &device,
			uint32_t queueFamilyIndex);

		virtual ~SCBBuilder() noexcept override;

		[[nodiscard]]
		std::future<VK::CommandBuffer *> build(
			std::function<void(VK::CommandBuffer &)> &&logic);

		[[nodiscard]]
		constexpr size_t getCapacity() const noexcept;

	private:
		Infra::MultiThreadPool __threadPool;

		std::vector<CommandBufferCirculator *> __circulators;
		size_t __circulatorCursor{ };
	};

	constexpr size_t SCBBuilder::getCapacity() const noexcept
	{
		return __threadPool.getPoolSize();
	}
}