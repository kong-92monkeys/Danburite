#pragma once

#include <Windows.h>
#include "../Infra/Stateful.h"
#include "../Infra/GLM.h"
#include "../Infra/Event.h"
#include "../Infra/ThreadPool.h"
#include "../Vulkan/Queue.h"
#include "../Vulkan/Surface.h"
#include "../Vulkan/Swapchain.h"
#include "../Vulkan/Image.h"
#include "../Vulkan/ImageView.h"
#include "../Vulkan/RenderPass.h"
#include "../Vulkan/Framebuffer.h"
#include "../Device/CommandBufferCirculator.h"
#include "../Device/SemaphoreCirculator.h"
#include "../Device/CommandExecutor.h"
#include "Layer.h"

namespace Render
{
	class RenderTarget : public Infra::Stateful<RenderTarget>
	{
	public:
		struct DrawResult
		{
		public:
			VK::CommandBuffer const *pCmdBuffer{ };
			VK::Swapchain *pSwapchain{ };
			uint32_t imageIndex{ };
			VK::Semaphore *pImageAcqSemaphore{ };
			VK::Semaphore *pSignalSemaphore{ };
		};

		RenderTarget(
			VK::Instance const &instance,
			VK::PhysicalDevice const &physicalDevice,
			VK::Device &device,
			VK::Queue &queue,
			Infra::DeferredDeleter &deferredDeleter,
			HINSTANCE hinstance,
			HWND hwnd);

		virtual ~RenderTarget() noexcept override;

		void addLayer(
			Layer *pLayer);

		void removeLayer(
			Layer *pLayer);

		void setBackgroundColor(
			glm::vec4 const &color) noexcept;

		void sync();

		[[nodiscard]]
		DrawResult draw();

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

	protected:
		virtual void _onValidate() override;

	private:
		VK::Instance const &__instance;
		VK::PhysicalDevice const &__physicalDevice;
		VK::Device &__device;
		VK::Queue &__que;
		Infra::DeferredDeleter &__deferredDeleter;

		std::unique_ptr<VK::Surface> __pSurface;

		VkSurfacePresentModeEXT __presentModeInfo{ };
		VkPhysicalDeviceSurfaceInfo2KHR __surfaceInfo{ };

		VkPresentModeKHR __presentMode{ };
		VkSurfaceCapabilities2KHR __capabilities{ };
		VkSurfaceFormatKHR __surfaceFormat{ };
		VkCompositeAlphaFlagBitsKHR __compositeAlpha{ };
		VkRect2D __renderArea{ };

		std::unique_ptr<VK::Swapchain> __pSwapchain;
		std::vector<std::unique_ptr<VK::Image>> __swapchainImages;
		std::vector<std::unique_ptr<VK::ImageView>> __swapchainImageViews;

		std::unique_ptr<VK::RenderPass> __pClearImageRenderPass;
		std::vector<std::unique_ptr<VK::Framebuffer>> __clearImageFramebuffers;

		std::unique_ptr<Dev::CommandBufferCirculator> __pDrawcallCmdBufferCirculator;
		std::unique_ptr<Dev::SemaphoreCirculator> __pImageAcqSemaphoreCirculator;
		std::unique_ptr<Dev::SemaphoreCirculator> __pCompleteSemaphoreCirculator;

		std::unique_ptr<RendererResourceManager> __pRendererResourceManager;

		std::unordered_set<Layer *> __layers;
		std::unordered_set<Layer *> __invalidatedLayers;

		bool __layerSortionInvalidated{ };
		std::vector<Layer *> __sortedLayers;

		glm::vec4 __backgroundColor{ 0.0f, 0.0f, 0.0f, 1.0f };

		Infra::EventListenerPtr<Layer *> __pLayerInvalidateListener;
		Infra::EventListenerPtr<Layer const *, int, int> __pLayerPriorityChangeListener;
		Infra::EventListenerPtr<Layer const *> __pLayerNeedRedrawListener;

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

		[[nodiscard]]
		VK::CommandBuffer &__beginNextDrawcallCmdBuffer();

		void __beginSwapchainImage(
			VK::CommandBuffer &cmdBuffer,
			uint32_t imageIndex);

		void __endSwapchainImage(
			VK::CommandBuffer &cmdBuffer,
			uint32_t imageIndex);

		void __sortLayers();

		void __onLayerInvalidated(
			Layer *pLayer) noexcept;

		void __onLayerPriorityChanged() noexcept;
		void __onLayerRedrawNeeded() const noexcept;
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