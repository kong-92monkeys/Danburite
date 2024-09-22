#pragma once

#include "../Infra/Version.h"
#include "../Vulkan/DebugUtilsMessenger.h"
#include "../Vulkan/PhysicalDevice.h"
#include <memory>
#include <vector>

namespace Dev
{
	class Context : public Infra::Unique
	{
	public:
		struct CreateInfo
		{
		public:
			std::string loaderLibName;
			bool debugMode{ };

			std::string engineName;
			Infra::Version engineVersion;

			std::string appName;
			Infra::Version appVersion;
		};

		explicit Context(
			CreateInfo const &createInfo);

		virtual ~Context() noexcept override;

		[[nodiscard]]
		constexpr VK::Instance const &getInstance() const noexcept;

		[[nodiscard]]
		constexpr size_t getPhysicalDeviceCount() const noexcept;

		[[nodiscard]]
		constexpr VK::PhysicalDevice const &getPhysicalDeviceOf(
			size_t index) const noexcept;

	private:
		std::unique_ptr<VK::VulkanLoader> __pLoader;
		std::unique_ptr<VK::Instance> __pInstance;

		VkDebugUtilsMessengerCreateInfoEXT __debugMessengerCreateInfo{ };
		std::unique_ptr<VK::DebugUtilsMessenger> __pDebugUtilsMessenger;

		std::vector<std::unique_ptr<VK::PhysicalDevice>> __physicalDevices;

		void __populateDebugMessengerCreateInfo();

		void __createInstance(
			CreateInfo const &createInfo);

		void __enumeratePhysicalDevices();

		static VkBool32 __debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageTypes,
			VkDebugUtilsMessengerCallbackDataEXT const *pCallbackData,
			void *pUserData) noexcept;
	};

	constexpr VK::Instance const &Context::getInstance() const noexcept
	{
		return *__pInstance;
	}

	constexpr size_t Context::getPhysicalDeviceCount() const noexcept
	{
		return __physicalDevices.size();
	}

	constexpr VK::PhysicalDevice const &Context::getPhysicalDeviceOf(
		size_t const index) const noexcept
	{
		return *(__physicalDevices[index]);
	}
}