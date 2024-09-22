#pragma once

#include "../Infra/Unique.h"
#include "Vulkan.h"
#include "VulkanProc.h"
#include <Windows.h>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>

namespace VK
{
	class VulkanLoader : public Infra::Unique
	{
	public:
		explicit VulkanLoader(
			std::string_view const &libName);

		virtual ~VulkanLoader() noexcept;

		[[nodiscard]]
		constexpr uint32_t getInstanceVersion() const noexcept;

		[[nodiscard]]
		const VkLayerProperties *getInstanceLayerOf(
			std::string_view const &name) const noexcept;

		[[nodiscard]]
		const VkExtensionProperties *getInstanceExtensionOf(
			std::string_view const &name) const noexcept;

		VkResult vkCreateInstance(
			VkInstanceCreateInfo const *pCreateInfo,
			VkAllocationCallbacks const *pAllocator,
			VkInstance *pInstance) const;

		[[nodiscard]]
		PFN_vkVoidFunction vkGetInstanceProcAddr(
			VkInstance instance,
			char const *pName) const;

	private:
		std::string const __libName;

		HMODULE __hLib{ };

		GlobalProc __globalProc;

		uint32_t __instanceVer{ };
		std::vector<VkLayerProperties> __instanceLayers;
		std::vector<VkExtensionProperties> __instanceExtensions;
		std::unordered_map<std::string_view, const VkLayerProperties *> __instanceLayerMap;
		std::unordered_map<std::string_view, const VkExtensionProperties *> __instanceExtensionMap;

		void __loadGlobalProc() noexcept;
		void __resolveInstanceVersion() noexcept;
		void __resolveInstanceLayers() noexcept;
		void __resolveInstanceExtensions() noexcept;
	};

	constexpr uint32_t VulkanLoader::getInstanceVersion() const noexcept
	{
		return __instanceVer;
	}
}