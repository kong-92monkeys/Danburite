#include "Queue.h"
#include <stdexcept>

namespace VK
{
	Queue::Queue(
		Device &device,
		uint32_t const familyIndex,
		uint32_t const queueIndex) :
		Handle			{ __retrieve(device, familyIndex, queueIndex) },
		__device		{ device },
		__familyIndex	{ familyIndex },
		__queueIndex	{ queueIndex }
	{}

	VkResult Queue::vkQueueWaitIdle()
	{
		return __device.vkQueueWaitIdle(getHandle());
	}

	VkResult Queue::vkQueueSubmit2(
		uint32_t const submitCount,
		VkSubmitInfo2 const *const pSubmits,
		VkFence const fence)
	{
		return __device.vkQueueSubmit2(
			getHandle(), submitCount, pSubmits, fence);
	}

	VkResult Queue::vkQueuePresentKHR(
		VkPresentInfoKHR const *const pPresentInfo)
	{
		return __device.vkQueuePresentKHR(
			getHandle(), pPresentInfo);
	}

	VkQueue Queue::__retrieve(
		Device &device,
		uint32_t const familyIndex,
		uint32_t const queueIndex)
	{
		const VkDeviceQueueInfo2 queueInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2 },
			.queueFamilyIndex	{ familyIndex },
			.queueIndex			{ queueIndex }
		};

		VkQueue retVal{ };
		device.vkGetDeviceQueue2(&queueInfo, &retVal);

		if (!retVal)
			throw std::runtime_error{ "Cannot retrieve a VkQueue." };

		return retVal;
	}
}