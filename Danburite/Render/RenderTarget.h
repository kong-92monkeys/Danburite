#pragma once

#include <Windows.h>
#include "../Infra/GLM.h"
#include "../Infra/Event.h"
#include "../Vulkan/Queue.h"
#include "../Vulkan/Surface.h"
#include "../Vulkan/Swapchain.h"
#include "../Vulkan/Image.h"
#include "../Vulkan/ImageView.h"
#include "../Vulkan/CommandBuffer.h"
#include "../Vulkan/Semaphore.h"
#include "../Vulkan/RenderPass.h"
#include "../Vulkan/Framebuffer.h"
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

		void sync();

		void setBackgroundColor(
			glm::vec4 const &color) noexcept;

		[[nodiscard]]
		uint32_t draw(
			VK::CommandBuffer &commandBuffer,
			VK::Semaphore &imageAcqSemaphore);

		[[nodiscard]]
		constexpr glm::vec4 const &getBackgroundColor() const noexcept;

		[[nodiscard]]
		constexpr VkExtent2D const &getExtent() const noexcept;

		[[nodiscard]]
		constexpr bool isPresentable() const noexcept;

		[[nodiscard]]
		constexpr uint32_t getSwapchainImageCount() const noexcept;

		[[nodiscard]]
		constexpr Infra::Event<RenderTarget const *> &getNeedRedrawEvent() const noexcept;

	private:
		VK::Instance &__instance;
		VK::PhysicalDevice &__physicalDevice;
		VK::Device &__device;
		VK::Queue &__que;

		std::unique_ptr<VK::Surface> __pSurface;

		VkSurfacePresentModeEXT __presentModeInfo{ };
		VkPhysicalDeviceSurfaceInfo2KHR __surfaceInfo{ };

		VkPresentModeKHR __presentMode{ };
		VkSurfaceCapabilities2KHR __capabilities{ };
		VkSurfaceFormatKHR __surfaceFormat{ };
		VkCompositeAlphaFlagBitsKHR __compositeAlpha{ };

		std::unique_ptr<VK::Swapchain> __pSwapchain;
		std::vector<std::unique_ptr<VK::Image>> __swapchainImages;
		std::vector<std::unique_ptr<VK::ImageView>> __swapchainImageViews;

		std::unique_ptr<VK::RenderPass> __pClearImageRenderPass;
		std::vector<std::unique_ptr<VK::Framebuffer>> __clearImageFramebuffers;

		glm::vec4 __backgroundColor{ 0.0f, 0.0f, 0.0f, 1.0f };

		mutable Infra::Event<RenderTarget const *> __needRedrawEvent;

		void __createSurface(
			HINSTANCE hinstance,
			HWND hwnd);

		void __createClearImageRenderPass();

		void __syncSurface();
		void __syncSwapchain();
		void __syncClearImageFramebuffers();

		void __verifySurfaceSupport();
		void __resolvePresentMode() noexcept;
		void __resolveCapabilities();
		void __resolveSurfaceFormat();
		void __resolveCompositeAlpha() noexcept;

		void __createSwapchain(
			VK::Swapchain *pOldSwapchain);

		void __enumerateSwapchainImages();
		void __createSwapchainImageViews();

		void __createClearImageFramebuffers();

		[[nodiscard]]
		uint32_t __acquireNextImage(
			VK::Semaphore &imageAcqSemaphore);

		void __readySwapchainImage(
			VK::CommandBuffer &commandBuffer,
			uint32_t imageIndex);
	};

	constexpr glm::vec4 const &RenderTarget::getBackgroundColor() const noexcept
	{
		return __backgroundColor;
	}

	constexpr VkExtent2D const &RenderTarget::getExtent() const noexcept
	{
		return __capabilities.surfaceCapabilities.currentExtent;
	}

	constexpr bool RenderTarget::isPresentable() const noexcept
	{
		auto const &extent{ getExtent() };
		return (extent.width && extent.height);
	}

	constexpr uint32_t RenderTarget::getSwapchainImageCount() const noexcept
	{
		return static_cast<uint32_t>(__swapchainImages.size());
	}

	constexpr Infra::Event<RenderTarget const *> &RenderTarget::getNeedRedrawEvent() const noexcept
	{
		return __needRedrawEvent;
	}
}