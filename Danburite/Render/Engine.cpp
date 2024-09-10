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
		auto const &deviceLimits{ __physicalDevice.getProps().p10->limits };

		__resolveQueueFamilyIndex();
		__createDevice();
		__createPipelineCache();

		__pMemoryAllocator = std::make_unique<Dev::MemoryAllocator>(
			__physicalDevice, *__pDevice,
			Constants::DEFAULT_MEMORY_BLOCK_SIZE,
			deviceLimits.minUniformBufferOffsetAlignment,
			deviceLimits.minStorageBufferOffsetAlignment);

		__pLayerResourcePool = std::make_unique<LayerResourcePool>(
			*__pDevice, __lazyDeleter, *__pMemoryAllocator);
	}

	Engine::~Engine() noexcept
	{
		__pLayerResourcePool = nullptr;
		__pMemoryAllocator = nullptr;
		__pPipelineCache = nullptr;
		__pDevice = nullptr;
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

			physicalDeviceFeatures.p12->imagelessFramebuffer &&
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
		features12.imagelessFramebuffer = VK_TRUE;
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

	void Engine::__createPipelineCache()
	{
		VkPipelineCacheCreateInfo const createInfo
		{
			.sType	{ VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO },
			.flags	{ VkPipelineCacheCreateFlagBits::VK_PIPELINE_CACHE_CREATE_EXTERNALLY_SYNCHRONIZED_BIT }
		};

		__pPipelineCache = std::make_unique<VK::PipelineCache>(*__pDevice, createInfo);
	}
}