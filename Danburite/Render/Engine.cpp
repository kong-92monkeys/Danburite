#include "Engine.h"
#include "../Device/ConversionUtil.h"
#include "Constants.h"
#include <stdexcept>
#include <format>

namespace Render
{
	Engine::Engine(
		Dev::Context &context,
		VK::PhysicalDevice &physicalDevice,
		GlobalDescriptorManager::BindingInfo const &globalDescBindingInfo) :
		__context			{ context },
		__physicalDevice	{ physicalDevice }
	{
		__verifyPhysicalDeviceSupport();
		__resolveQueueFamilyIndex();
		__createDevice();
		__retrieveQueue();
		__createPipelineCache();
		__createSubmissionFences();
		__createSubLayerDescLayout();

		auto const &deviceLimits{ __physicalDevice.getProps().p10->limits };

		__pMemoryAllocator = std::make_unique<Dev::MemoryAllocator>(
			__physicalDevice, *__pDevice,
			Constants::DEFAULT_MEMORY_BLOCK_SIZE,
			deviceLimits.minUniformBufferOffsetAlignment,
			deviceLimits.minStorageBufferOffsetAlignment);

		__pDescriptorUpdater = std::make_unique<Dev::DescriptorUpdater>(*__pDevice);

		__pResourcePool = std::make_unique<ResourcePool>(
			*__pDevice, __deferredDeleter, *__pMemoryAllocator);

		__pCommandSubmitter = std::make_unique<CommandSubmitter>(*__pQueue);

		__pGlobalDescriptorManager = std::make_unique<GlobalDescriptorManager>(
			__physicalDevice, *__pDevice, __deferredDeleter, __imageReferenceManager,
			*__pDescriptorUpdater, *__pResourcePool, globalDescBindingInfo);

		__pExecutorCmdBufferCirculator = std::make_unique<Dev::CommandBufferCirculator>(
			*__pDevice, __queueFamilyIndex,
			VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY, 2U, 30U);
	}

	Engine::~Engine() noexcept
	{
		__pDevice->vkDeviceWaitIdle();

		__pExecutorCmdBufferCirculator = nullptr;
		__pGlobalDescriptorManager = nullptr;
		__pCommandSubmitter = nullptr;
		__pResourcePool = nullptr;
		__pDescriptorUpdater = nullptr;

		__deferredDeleter.flush();
		// DO NOT DELETE deferredDeleter-dependent modules from now

		__pMemoryAllocator = nullptr;
		__pSubLayerDescSetLayout = nullptr;
		__submissionFences.clear();
		__pPipelineCache = nullptr;

		__pQueue = nullptr;
		__pDevice = nullptr;
	}

	std::unique_ptr<RenderTarget> Engine::createRenderTarget(
		HINSTANCE const hinstance,
		HWND const hwnd)
	{
		return std::make_unique<RenderTarget>(
			__context.getInstance(), __physicalDevice, *__pDevice, *__pQueue,
			__deferredDeleter, hinstance, hwnd);
	}

	std::shared_ptr<Layer> Engine::createLayer()
	{
		return std::make_shared<Layer>(
			*__pDevice, *__pSubLayerDescSetLayout, __deferredDeleter,
			*__pDescriptorUpdater, *__pResourcePool, *__pGlobalDescriptorManager);
	}

	std::shared_ptr<Mesh> Engine::createMesh()
	{
		return std::make_shared<Mesh>(
			*__pDevice, *__pMemoryAllocator,
			__commandExecutor, *__pResourcePool);
	}

	std::shared_ptr<Texture> Engine::createTexture(
		Texture::ImageCreateInfo const &imageCreateInfo,
		Texture::ImageViewCreateInfo const &imageViewCreateInfo)
	{
		return std::make_shared<Texture>(
			*__pDevice, __commandExecutor,
			*__pMemoryAllocator, __deferredDeleter, *__pResourcePool,
			imageCreateInfo, imageViewCreateInfo);
	}

	void Engine::reserveRender(
		RenderTarget *const pRenderTarget) noexcept
	{
		__reservedRenderTargets.emplace(pRenderTarget);
	}

	void Engine::render()
	{
		__pGlobalDescriptorManager->validate();
		__validateReservedRenderTargets();

		__pDescriptorUpdater->update();
		__pCommandSubmitter->clear();

		auto &executorCmdBuffer{ __beginNextExecutorCmdBuffer() };
		__commandExecutor.execute(executorCmdBuffer);
		executorCmdBuffer.vkEndCommandBuffer();

		__pCommandSubmitter->addGeneralExecution(executorCmdBuffer);

		for (auto const pRenderTarget : __reservedRenderTargets)
			__pCommandSubmitter->addDrawResult(pRenderTarget->draw());

		auto &submissionFence{ __getNextSubmissionFence() };
		__pDevice->vkResetFences(1U, &(submissionFence.getHandle()));

		__pCommandSubmitter->submit(submissionFence);
		__pCommandSubmitter->present();

		__deferredDeleter.advance();
		__reservedRenderTargets.clear();
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

			physicalDeviceFeatures.pRobustness2->nullDescriptor &&
			physicalDeviceFeatures.pNestedCommandBuffer->nestedCommandBuffer
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
		VkPhysicalDeviceNestedCommandBufferFeaturesEXT featuresNestedCommandBuffer{ };

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
		featuresRobustness.pNext = &featuresNestedCommandBuffer;

		featuresNestedCommandBuffer.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_NESTED_COMMAND_BUFFER_FEATURES_EXT;
		featuresNestedCommandBuffer.nestedCommandBuffer = VK_TRUE;
		featuresNestedCommandBuffer.pNext = nullptr;

		std::vector<char const *> extensions;
		extensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		extensions.emplace_back(VK_KHR_MAINTENANCE_5_EXTENSION_NAME);
		extensions.emplace_back(VK_EXT_ROBUSTNESS_2_EXTENSION_NAME);
		extensions.emplace_back(VK_EXT_NESTED_COMMAND_BUFFER_EXTENSION_NAME);
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

	void Engine::__createSubmissionFences()
	{
		VkFenceCreateInfo const createInfo
		{
			.sType	{ VkStructureType::VK_STRUCTURE_TYPE_FENCE_CREATE_INFO },
			.flags	{ VkFenceCreateFlagBits::VK_FENCE_CREATE_SIGNALED_BIT }
		};

		for (size_t iter{ }; iter < Constants::MAX_IN_FLIGHT_FRAME_COUNT; ++iter)
			__submissionFences.emplace_back(std::make_unique<VK::Fence>(*__pDevice, createInfo));
	}

	void Engine::__createSubLayerDescLayout()
	{
		std::vector<VkDescriptorBindingFlags> bindingFlags;
		std::vector<VkDescriptorSetLayoutBinding> bindings;

		auto &materialBufferBinding				{ bindings.emplace_back() };
		materialBufferBinding.binding			= Constants::SUB_LAYER_DESC_INSTANCE_INFO_LOCATION;
		materialBufferBinding.descriptorType	= VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		materialBufferBinding.descriptorCount	= 1U;
		materialBufferBinding.stageFlags		= VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;
			
		bindingFlags.emplace_back(0U);

		VkDescriptorSetLayoutBindingFlagsCreateInfo const flagInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO },
			.bindingCount	{ static_cast<uint32_t>(bindingFlags.size()) },
			.pBindingFlags	{ bindingFlags.data() }
		};

		VkDescriptorSetLayoutCreateInfo const createInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO },
			.pNext			{ &flagInfo },
			.bindingCount	{ static_cast<uint32_t>(bindings.size()) },
			.pBindings		{ bindings.data() }
		};

		__pSubLayerDescSetLayout = std::make_unique<VK::DescriptorSetLayout>(*__pDevice, createInfo);
	}

	VK::CommandBuffer &Engine::__beginNextExecutorCmdBuffer()
	{
		VK::CommandBuffer &retVal{ __pExecutorCmdBufferCirculator->getNext() };

		VkCommandBufferBeginInfo const cbBeginInfo
		{
			.sType{ VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO },
			.flags{ VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT }
		};

		auto const result{ retVal.vkBeginCommandBuffer(&cbBeginInfo) };
		if (result != VkResult::VK_SUCCESS)
			throw std::runtime_error{ "Cannot begin a executor command buffer." };

		return retVal;
	}

	VK::Fence &Engine::__getNextSubmissionFence()
	{
		auto &retVal{ *(__submissionFences[__submissionFenceCursor]) };
		__submissionFenceCursor = ((__submissionFenceCursor + 1ULL) % __submissionFences.size());

		auto const result
		{
			__pDevice->vkWaitForFences(
				1U, &(retVal.getHandle()), VK_FALSE, UINT64_MAX)
		};
		
		if (result != VkResult::VK_SUCCESS)
			throw std::runtime_error{ "Cannot resolve a submission fence." };

		return retVal;
	}

	void Engine::__validateReservedRenderTargets()
	{
		for (auto it{ __reservedRenderTargets.begin() }; it != __reservedRenderTargets.end(); )
		{
			auto const pRenderTarget{ *it };

			if (pRenderTarget->isPresentable())
			{
				pRenderTarget->validate();
				++it;
			}
			else
				it = __reservedRenderTargets.erase(it);
		}
	}
}