#include "SCBBuilder.h"

namespace Dev
{
	SCBBuilder::SCBBuilder(
		VK::Device &device,
		uint32_t const queueFamilyIndex)
	{
		size_t const poolSize{ __threadPool.getPoolSize() };
		for (size_t poolIter{ }; poolIter < poolSize; ++poolIter)
		{
			auto const pCirculator
			{
				new CommandBufferCirculator
				{
					device, queueFamilyIndex,
					VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_SECONDARY,
					3, 100U
				}
			};

			__circulators.emplace_back(pCirculator);
		}
	}

	SCBBuilder::~SCBBuilder() noexcept
	{
		for (auto const pCirculator : __circulators)
			delete pCirculator;
	}

	std::future<VK::CommandBuffer *> SCBBuilder::build(
		std::function<void(VK::CommandBuffer &)> &&logic)
	{
		auto const pPromise	{ new std::promise<VK::CommandBuffer *> };
		auto retVal			{ pPromise->get_future() };

		__threadPool.silentRun(__circulatorCursor,
			[logic{ std::move(logic) }, &cmdBuffer{ __circulators[__circulatorCursor]->getNext() }, pPromise]
		{
			logic(cmdBuffer);
			pPromise->set_value(&cmdBuffer);
			delete pPromise;
		});

		__circulatorCursor = ((__circulatorCursor + 1ULL) % __threadPool.getPoolSize());
		return retVal;
	}
}