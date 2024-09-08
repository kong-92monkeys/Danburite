#pragma warning(disable: 5103)

#include "VulkanLoader.h"
#include <stdexcept>
#include <format>

#define LOAD_GLOBAL_PROC(funcName) (__globalProc.##funcName = reinterpret_cast<PFN_##funcName>(vkGetInstanceProcAddr(nullptr, #funcName)))

namespace VK
{
	VulkanLoader::VulkanLoader(
		std::string_view const &libName) :
		__libName	{ libName }
	{
		__hLib = LoadLibraryA(__libName.c_str());
		if (!__hLib)
			throw std::runtime_error{ std::format("Cannot load vulkan loader with the given name: {}", __libName) };

		__loadGlobalProc();
		__resolveInstanceVersion();
		__resolveInstanceLayers();
		__resolveInstanceExtensions();
	}

	VulkanLoader::~VulkanLoader() noexcept
	{
		if (__hLib)
			FreeLibrary(__hLib);
	}

	const VkLayerProperties *VulkanLoader::getInstanceLayerOf(
		std::string_view const &name) const noexcept
	{
		auto const foundIt{ __instanceLayerMap.find(name) };
		if (foundIt == __instanceLayerMap.end())
			return nullptr;

		return foundIt->second;
	}

	const VkExtensionProperties *VulkanLoader::getInstanceExtensionOf(
		std::string_view const &name) const noexcept
	{
		auto const foundIt{ __instanceExtensionMap.find(name) };
		if (foundIt == __instanceExtensionMap.end())
			return nullptr;

		return foundIt->second;
	}

	VkResult VulkanLoader::vkCreateInstance(
		VkInstanceCreateInfo const *const pCreateInfo,
		VkAllocationCallbacks const *const pAllocator,
		VkInstance *const pInstance)
	{
		return __globalProc.vkCreateInstance(pCreateInfo, pAllocator, pInstance);
	}

	PFN_vkVoidFunction VulkanLoader::vkGetInstanceProcAddr(
		VkInstance const instance,
		char const *const pName)
	{
		return __globalProc.vkGetInstanceProcAddr(instance, pName);
	}

	void VulkanLoader::__loadGlobalProc() noexcept
	{
		auto const vkGetInstanceProcAddr
		{
			reinterpret_cast<PFN_vkGetInstanceProcAddr>(
				GetProcAddress(__hLib, "vkGetInstanceProcAddr"))
		};

		__globalProc.vkGetInstanceProcAddr = vkGetInstanceProcAddr;

		LOAD_GLOBAL_PROC(vkEnumerateInstanceVersion);
		LOAD_GLOBAL_PROC(vkEnumerateInstanceExtensionProperties);
		LOAD_GLOBAL_PROC(vkEnumerateInstanceLayerProperties);
		LOAD_GLOBAL_PROC(vkCreateInstance);
	}

	void VulkanLoader::__resolveInstanceVersion() noexcept
	{
		if (!(__globalProc.vkEnumerateInstanceVersion))
			__instanceVer = VK_API_VERSION_1_0;
		else
			__globalProc.vkEnumerateInstanceVersion(&__instanceVer);
	}

	void VulkanLoader::__resolveInstanceLayers() noexcept
	{
		uint32_t layerCount{ };
		__globalProc.vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		__instanceLayers.resize(layerCount);
		__globalProc.vkEnumerateInstanceLayerProperties(&layerCount, __instanceLayers.data());

		for (const auto &layer : __instanceLayers)
			__instanceLayerMap[layer.layerName] = &layer;
	}

	void VulkanLoader::__resolveInstanceExtensions() noexcept
	{
		uint32_t extensionCount{ };
		__globalProc.vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		__instanceExtensions.resize(extensionCount);
		__globalProc.vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, __instanceExtensions.data());

		for (const auto &extension : __instanceExtensions)
			__instanceExtensionMap[extension.extensionName] = &extension;
	}
}