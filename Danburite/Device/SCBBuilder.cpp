#include "SCBBuilder.h"

namespace Dev
{
	SCBBuilder::SCBBuilder(
		VK::Device &device,
		uint32_t const queueFamilyIndex)
	{
		__pCirculator = std::make_unique<CommandBufferCirculator>(
			device, queueFamilyIndex,
			VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_SECONDARY, 100U, 200U);
	}

	std::future<VK::CommandBuffer *> SCBBuilder::build(
		std::function<void(VK::CommandBuffer &)> &&logic)
	{
		auto const pPromise	{ new std::promise<VK::CommandBuffer *> };
		auto retVal			{ pPromise->get_future() };

		__threadPool.silentRun(
			[logic{ std::move(logic) }, &cmdBuffer{ __pCirculator->getNext() }, pPromise]
		{
			logic(cmdBuffer);
			pPromise->set_value(&cmdBuffer);
			delete pPromise;
		});

		return retVal;
	}
}