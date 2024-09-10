#include "Context.h"
#include "ConversionUtil.h"
#include "../Infra/Logger.h"
#include <unordered_map>
#include <stdexcept>
#include <array>

namespace Dev
{
	Context::Context(
		CreateInfo const &createInfo)
	{
		__pLoader = std::make_unique<VK::VulkanLoader>(createInfo.loaderLibName);

		if (createInfo.debugMode)
			__populateDebugMessengerCreateInfo();

		__createInstance(createInfo);

		if (createInfo.debugMode)
			__pDebugUtilsMessenger = std::make_unique<VK::DebugUtilsMessenger>(*__pInstance, __debugMessengerCreateInfo);

		__enumeratePhysicalDevices();
	}

	Context::~Context() noexcept
	{
		__pDebugUtilsMessenger = nullptr;
		__pInstance = nullptr;
		__pLoader = nullptr;
	}

	void Context::__populateDebugMessengerCreateInfo()
	{
		__debugMessengerCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		__debugMessengerCreateInfo.messageSeverity = (
			VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
			VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
		);

		__debugMessengerCreateInfo.messageType = (
			VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
			VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT
		);

		__debugMessengerCreateInfo.pfnUserCallback = __debugCallback;
	}

	void Context::__createInstance(
		CreateInfo const &contextCreateInfo)
	{
		void const *pNextChain{ };
		std::vector<char const *> layers;
		std::vector<char const *> extensions;

		std::vector<VkValidationFeatureEnableEXT> enabledFeatures;
		VkValidationFeaturesEXT validationFeatures{ };

		auto const instanceVer{ ConversionUtil::fromVulkanVersion(__pLoader->getInstanceVersion()) };
		if (instanceVer < ConversionUtil::fromVulkanVersion(VK_API_VERSION_1_3))
			throw std::runtime_error{ "The supported vulkan instance version is too low." };

		extensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
		extensions.emplace_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
		extensions.emplace_back(VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME);

		if (contextCreateInfo.debugMode)
		{
			layers.emplace_back("VK_LAYER_KHRONOS_validation");
			extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

			enabledFeatures.emplace_back(VkValidationFeatureEnableEXT::VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT);
			// enabledFeatures.emplace_back(VkValidationFeatureEnableEXT::VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT);

			validationFeatures.sType							= VkStructureType::VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
			validationFeatures.pNext							= &__debugMessengerCreateInfo;
			validationFeatures.enabledValidationFeatureCount	= static_cast<uint32_t>(enabledFeatures.size());
			validationFeatures.pEnabledValidationFeatures		= enabledFeatures.data();

			pNextChain = &validationFeatures;
		}

		for (auto const layer : layers)
		{
			if (__pLoader->getInstanceLayerOf(layer))
				continue;

			throw std::runtime_error{ std::format("Instance layer not supported: {}", layer) };
		}

		for (auto const extension : extensions)
		{
			if (__pLoader->getInstanceExtensionOf(extension))
				continue;

			throw std::runtime_error{ std::format("Instance extension not supported: {}", extension) };
		}

		VkApplicationInfo const appInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_APPLICATION_INFO },
			.pApplicationName	{ contextCreateInfo.appName.c_str() },
			.applicationVersion	{ ConversionUtil::toVulkanVersion(contextCreateInfo.appVersion) },
			.pEngineName		{ contextCreateInfo.engineName.c_str() },
			.engineVersion		{ ConversionUtil::toVulkanVersion(contextCreateInfo.engineVersion) },
			.apiVersion			{ __pLoader->getInstanceVersion() }
		};

		VkInstanceCreateInfo const instanceCreateInfo
		{
			.sType						{ VkStructureType::VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO },
			.pNext						{ pNextChain },
			.pApplicationInfo			{ &appInfo },
			.enabledLayerCount			{ static_cast<uint32_t>(layers.size()) },
			.ppEnabledLayerNames		{ layers.data() },
			.enabledExtensionCount		{ static_cast<uint32_t>(extensions.size()) },
			.ppEnabledExtensionNames	{ extensions.data() }
		};

		__pInstance = std::make_unique<VK::Instance>(*__pLoader, instanceCreateInfo);
	}

	void Context::__enumeratePhysicalDevices()
	{
		std::vector<VkPhysicalDevice> handles;

		uint32_t deviceCount{ };
		__pInstance->vkEnumeratePhysicalDevices(&deviceCount, nullptr);

		if (!deviceCount)
			throw std::runtime_error{ "No physical devices are detected." };

		handles.resize(deviceCount);
		__pInstance->vkEnumeratePhysicalDevices(&deviceCount, handles.data());

		for (auto const handle : handles)
			__physicalDevices.emplace_back(std::make_unique<VK::PhysicalDevice>(*__pInstance, handle));
	}

	VkBool32 Context::__debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT const messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT const messageTypes,
		VkDebugUtilsMessengerCallbackDataEXT const *const pCallbackData,
		void *const pUserData) noexcept
	{
		Infra::Logger::Severity logSeverity{ };

		switch (messageSeverity)
		{
			case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
				logSeverity = Infra::Logger::Severity::FATAL;
				break;

			case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
				logSeverity = Infra::Logger::Severity::WARNING;
				break;

			case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
				logSeverity = Infra::Logger::Severity::INFO;
				break;

			case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
				logSeverity = Infra::Logger::Severity::VERBOSE;
				break;
		}

		Infra::Logger::log(logSeverity, pCallbackData->pMessage);
		return VK_FALSE;
	}
}