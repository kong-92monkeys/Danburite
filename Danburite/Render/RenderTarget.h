#pragma once

#include <Windows.h>
#include "../Infra/GLM.h"
#include "../Vulkan/Queue.h"
#include "../Vulkan/Surface.h"
#include "../Vulkan/Swapchain.h"
#include <memory>

namespace Render
{
	class RenderTarget : public Infra::Unique
	{
	public:
		RenderTarget(
			VK::Instance &instance,
			VK::PhysicalDevice &physicalDevice,
			VK::Device &device,
			VK::Queue &queue,
			HINSTANCE hinstance,
			HWND hwnd);

		virtual ~RenderTarget() noexcept override;

		[[nodiscard]]
		constexpr VkExtent2D const &getExtent() const noexcept;

		[[nodiscard]]
		constexpr bool isPresentable() const noexcept;

		void sync();

	private:
		VK::Instance &__instance;
		VK::PhysicalDevice &__physicalDevice;
		VK::Device &__device;
		VK::Queue &__que;

		std::unique_ptr<VK::Surface> __pSurface;
		std::unique_ptr<VK::Swapchain> __pSwapchain;

		VkSurfacePresentModeEXT __presentModeInfo{ };
		VkPhysicalDeviceSurfaceInfo2KHR __surfaceInfo{ };

		VkPresentModeKHR __presentMode{ };
		VkSurfaceCapabilities2KHR __capabilities{ };
		VkSurfaceFormatKHR __surfaceFormat{ };
		VkCompositeAlphaFlagBitsKHR __compositeAlpha{ };

		void __createSurface(
			HINSTANCE hinstance,
			HWND hwnd);

		void __syncSurface();
		void __syncSwapchain();

		void __verifySurfaceSupport();
		void __resolvePresentMode() noexcept;
		void __resolveCapabilities();
		void __resolveSurfaceFormat();
		void __resolveCompositeAlpha() noexcept;

		void __createSwapchain(
			VK::Swapchain *pOldSwapchain);
	};

	constexpr VkExtent2D const &RenderTarget::getExtent() const noexcept
	{
		return __capabilities.surfaceCapabilities.currentExtent;
	}

	constexpr bool RenderTarget::isPresentable() const noexcept
	{
		auto const &extent{ getExtent() };
		return (extent.width && extent.height);
	}
}