#include "Engine.h"
#include "../Device/ConversionUtil.h"
#include "Constants.h"
#include <stdexcept>
#include <format>

namespace Render
{
	Engine::Engine(
		Dev::Context &context,
		VK::PhysicalDevice &physicalDevice) :
		__context			{ context },
		__physicalDevice	{ physicalDevice }
	{
		__verifyPhysicalDeviceSupport();
		__resolveQueueFamilyIndex();
		__createDevice();
		__retrieveQueue();
		__createPipelineCache();
		__createRenderTargetDescSetLayout();

		auto const &deviceLimits{ __physicalDevice.getProps().p10->limits };

		__pMemoryAllocator = std::make_unique<Dev::MemoryAllocator>(
			__physicalDevice, *__pDevice,
			Constants::DEFAULT_MEMORY_BLOCK_SIZE,
			deviceLimits.minUniformBufferOffsetAlignment,
			deviceLimits.minStorageBufferOffsetAlignment);

		__pPrimaryCmdBufferCirculator = std::make_unique<Dev::CommandBufferCirculator>(
			*__pDevice, __queueFamilyIndex,
			VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY, 2U, 30U);

		__pSubmissionFenceCirculator = std::make_unique<Dev::FenceCirculator>(
			*__pDevice, Constants::MAX_IN_FLIGHT_FRAME_COUNT_LIMIT);

		__pImageAcqSemaphoreCirculator = std::make_unique<Dev::SemaphoreCirculator>(
			*__pDevice, VkSemaphoreType::VK_SEMAPHORE_TYPE_BINARY, 30ULL);

		__pSubmissionSemaphoreCirculator = std::make_unique<Dev::SemaphoreCirculator>(
			*__pDevice, VkSemaphoreType::VK_SEMAPHORE_TYPE_BINARY, Constants::MAX_IN_FLIGHT_FRAME_COUNT_LIMIT);

		__pLayerResourcePool = std::make_unique<LayerResourcePool>(
			*__pDevice, __lazyDeleter, *__pMemoryAllocator);

		/*
			TODO: Renderer dependent resources
				- render pass
				- descriptor layout, pool, set
		*/
	}

	Engine::~Engine() noexcept
	{
		__lazyDeleter.flush();
		__pDevice->vkDeviceWaitIdle();

		__pLayerResourcePool = nullptr;

		__pSubmissionSemaphoreCirculator = nullptr;
		__pImageAcqSemaphoreCirculator = nullptr;
		__pSubmissionFenceCirculator = nullptr;
		__pPrimaryCmdBufferCirculator = nullptr;

		__pMemoryAllocator = nullptr;
		__pRenderTargetDescSetLayout = nullptr;
		__pPipelineCache = nullptr;

		__pQueue = nullptr;
		__pDevice = nullptr;
	}

	std::unique_ptr<RenderTarget> Engine::createRenderTarget(
		HINSTANCE const hinstance,
		HWND const hwnd)
	{
		return std::make_unique<RenderTarget>(
			__context.getInstance(), __physicalDevice,
			*__pDevice, *__pQueue, hinstance, hwnd);
	}

	void Engine::render(
		RenderTarget &renderTarget)
	{
		if (!(renderTarget.isPresentable()))
			return;

		auto &cmdBuffer				{ __pPrimaryCmdBufferCirculator->getNext() };
		auto &imageAcqSemaphore		{ __pImageAcqSemaphoreCirculator->getNext() };
		auto &submissonSemaphore	{ __pSubmissionSemaphoreCirculator->getNext() };

		uint32_t const imageIndex{ __recordPrimaryCmdBuffer(cmdBuffer, imageAcqSemaphore, renderTarget) };
		__submitPrimaryCmdBuffer(cmdBuffer, imageAcqSemaphore, submissonSemaphore);

		__lazyDeleter.advance();

		renderTarget.present(submissonSemaphore, imageIndex);
	}

	void Engine::__verifyPhysicalDeviceSupport()
	{
		auto const &physicalDeviceProps		{ __physicalDevice.getProps() };
		auto const &physicalDeviceFeatures	{ __physicalDevice.getFeatures() };

		auto const deviceVersion
		{
			Dev::ConversionUtil::fromVulkanVersion(
				physicalDeviceProps.p10->apiVersion)
		};
		
		if ((deviceVersion.major <= 1U) && (deviceVersion.minor < 3U))
			throw std::runtime_error{ "The device API version is too low." };

		bool const featureSupported
		{
			physicalDeviceFeatures.p10->samplerAnisotropy &&

			physicalDeviceFeatures.p12->descriptorBindingVariableDescriptorCount &&
			physicalDeviceFeatures.p12->descriptorBindingPartiallyBound &&
			physicalDeviceFeatures.p12->runtimeDescriptorArray &&

			physicalDeviceFeatures.p13->pipelineCreationCacheControl &&
			physicalDeviceFeatures.p13->synchronization2 &&

			physicalDeviceFeatures.pRobustness2->nullDescriptor
		};

		if (!featureSupported)
			throw std::runtime_error{ "The device doesn't support the features." };

		if (!(__physicalDevice.vkGetPhysicalDeviceWin32PresentationSupportKHR(__queueFamilyIndex)))
			throw std::runtime_error{ "The device doesn't support win32 presentation." };
	}

	void Engine::__resolveQueueFamilyIndex()
	{
		auto const &familyInfos{ __physicalDevice.getQueueFamilyInfos() };

		std::optional<uint32_t> familyIndex;

		const uint32_t familyCount{ static_cast<uint32_t>(familyInfos.size()) };
		for (uint32_t iter{ }; iter < familyCount; ++iter)
		{
			auto const &familyInfo	{ familyInfos[iter] };
			auto const queueFlags	{ familyInfo.pProps->queueFlags };

			if (!(queueFlags & VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT))
				continue;

			familyIndex = iter;
			break;
		}

		if (familyIndex.has_value())
		{
			__queueFamilyIndex = familyIndex.value();
			return;
		}

		throw std::runtime_error{ "Cannot resolve suitable queue family index." };
	}

	void Engine::__createDevice()
	{
		VkPhysicalDeviceFeatures2 features{ };
		VkPhysicalDeviceVulkan11Features features11{ };
		VkPhysicalDeviceVulkan12Features features12{ };
		VkPhysicalDeviceVulkan13Features features13{ };
		VkPhysicalDeviceRobustness2FeaturesEXT featuresRobustness{ };

		features.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		features.features.samplerAnisotropy = VK_TRUE;
		features.pNext = &features11;

		features11.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
		features11.pNext = &features12;

		features12.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
		features12.descriptorBindingVariableDescriptorCount = VK_TRUE;
		features12.descriptorBindingPartiallyBound = VK_TRUE;
		features12.runtimeDescriptorArray = VK_TRUE;
		features12.pNext = &features13;
		
		features13.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
		features13.pipelineCreationCacheControl = VK_TRUE;
		features13.synchronization2 = VK_TRUE;
		features13.pNext = &featuresRobustness;

		featuresRobustness.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT;
		featuresRobustness.nullDescriptor = VK_TRUE;
		featuresRobustness.pNext = nullptr;

		std::vector<char const *> extensions;
		extensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		extensions.emplace_back(VK_KHR_MAINTENANCE_5_EXTENSION_NAME);
		extensions.emplace_back(VK_EXT_ROBUSTNESS_2_EXTENSION_NAME);
		//extensions.emplace_back(VK_EXT_IMAGE_COMPRESSION_CONTROL_EXTENSION_NAME);

		for (auto const extension : extensions)
		{
			if (__physicalDevice.getExtensionOf(extension))
				continue;

			throw std::runtime_error{ std::format("Device extension not supported: {}", extension) };
		}

		static constexpr float queuePriority{ 1.0f };

		VkDeviceQueueCreateInfo const queueCreateInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO },
			.queueFamilyIndex	{ __queueFamilyIndex },
			.queueCount			{ 1U },
			.pQueuePriorities	{ &queuePriority }
		};

		VkDeviceCreateInfo const createInfo
		{
			.sType						{ VkStructureType::VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO },
			.pNext						{ &features },
			.queueCreateInfoCount		{ 1U },
			.pQueueCreateInfos			{ &queueCreateInfo },
			.enabledExtensionCount		{ static_cast<uint32_t>(extensions.size()) },
			.ppEnabledExtensionNames	{ extensions.data() }
		};

		__pDevice = std::make_unique<VK::Device>(__physicalDevice, createInfo);
	}

	void Engine::__retrieveQueue()
	{
		__pQueue = std::make_unique<VK::Queue>(*__pDevice, __queueFamilyIndex, 0U);
	}

	void Engine::__createPipelineCache()
	{
		VkPipelineCacheCreateInfo const createInfo
		{
			.sType	{ VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO },
			.flags	{ VkPipelineCacheCreateFlagBits::VK_PIPELINE_CACHE_CREATE_EXTERNALLY_SYNCHRONIZED_BIT }
		};

		__pPipelineCache = std::make_unique<VK::PipelineCache>(*__pDevice, createInfo);
	}

	void Engine::__createRenderTargetDescSetLayout()
	{
		std::vector<VkDescriptorSetLayoutBinding> bindings;

		VkDescriptorSetLayoutCreateInfo const createInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO },
			.bindingCount		{ static_cast<uint32_t>(bindings.size()) },
			.pBindings			{ bindings.data() }
		};

		__pRenderTargetDescSetLayout = std::make_unique<VK::DescriptorSetLayout>(*__pDevice, createInfo);
	}

	uint32_t Engine::__recordPrimaryCmdBuffer(
		VK::CommandBuffer &cmdBuffer,
		VK::Semaphore &imageAcqSemaphore,
		RenderTarget &renderTarget)
	{
		VkCommandBufferBeginInfo const cbBeginInfo
		{
			.sType	{ VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO },
			.flags	{ VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT }
		};

		cmdBuffer.vkBeginCommandBuffer(&cbBeginInfo);

		 __commandExecutor.execute(cmdBuffer);
		uint32_t const imageIndex{ renderTarget.draw(cmdBuffer, imageAcqSemaphore) };

		cmdBuffer.vkEndCommandBuffer();

		return imageIndex;
	}

	void Engine::__submitPrimaryCmdBuffer(
		VK::CommandBuffer &cmdBuffer,
		VK::Semaphore &imageAcqSemaphore,
		VK::Semaphore &submissonSemaphore)
	{
		VkCommandBufferSubmitInfo const cmdBufferInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO },
			.commandBuffer	{ cmdBuffer.getHandle() }
		};

		VkSemaphoreSubmitInfo const waitSemaphoreInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO },
			.semaphore		{ imageAcqSemaphore.getHandle() },
			.stageMask		{ VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT }
		};

		VkSemaphoreSubmitInfo const signalSemaphoreInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO },
			.semaphore		{ submissonSemaphore.getHandle() },
			.stageMask		{ VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT }
		};

		VkSubmitInfo2 const submitInfo
		{
			.sType						{ VkStructureType::VK_STRUCTURE_TYPE_SUBMIT_INFO_2 },
			.waitSemaphoreInfoCount		{ 1U },
			.pWaitSemaphoreInfos		{ &waitSemaphoreInfo },
			.commandBufferInfoCount		{ 1U },
			.pCommandBufferInfos		{ &cmdBufferInfo },
			.signalSemaphoreInfoCount	{ 1U },
			.pSignalSemaphoreInfos		{ &signalSemaphoreInfo }
		};

		auto &fence{ __getNextSubmissionFence() };
		__pDevice->vkResetFences(1U, &(fence.getHandle()));

		__pQueue->vkQueueSubmit2(1U, &submitInfo, fence.getHandle());
	}

	VK::Fence &Engine::__getNextSubmissionFence() noexcept
	{
		for (auto it{ __inFlightFences.begin() }; it != __inFlightFences.end(); ++it)
		{
			auto const pFence{ *it };

			auto const result
			{
				__pDevice->vkWaitForFences(
					1U, &(pFence->getHandle()), VK_FALSE, 0ULL)
			};

			if (result == VK_SUCCESS)
			{
				__inFlightFences.erase(it);
				return *pFence;
			}
		}

		if (__inFlightFences.size() < __maxInFlightSubmissionCount)
		{
			auto &nextFence{ __pSubmissionFenceCirculator->getNext() };
			__inFlightFences.emplace(&nextFence);
			return nextFence;
		}

		VK::Fence *pRetVal{ };
		while (!pRetVal)
		{
			for (auto it{ __inFlightFences.begin() }; it != __inFlightFences.end(); ++it)
			{
				const auto pFence{ *it };

				auto const result
				{
					// 1ms
					__pDevice->vkWaitForFences(
						1U, &(pFence->getHandle()), VK_FALSE, 1'000'000ULL)
				};

				if (result == VK_SUCCESS)
				{
					__inFlightFences.erase(it);
					pRetVal = pFence;
					break;
				}
			}
		}

		return *pRetVal;
	}
}