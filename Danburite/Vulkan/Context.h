#pragma once

#include "../Infra/Version.h"
#include "Instance.h"
#include "DebugUtilsMessenger.h"
#include "PhysicalDevice.h"
#include <memory>
#include <vector>

namespace VK
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

	private:
		std::unique_ptr<VulkanLoader> __pLoader;
		std::unique_ptr<Instance> __pInstance;

		VkDebugUtilsMessengerCreateInfoEXT __debugMessengerCreateInfo{ };
		std::unique_ptr<DebugUtilsMessenger> __pDebugUtilsMessenger;

		std::vector<std::unique_ptr<PhysicalDevice>> __physicalDevices;

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
}