#include "RenderTarget.h"
#include "Constants.h"
#include <stdexcept>
#include <algorithm>
#include "SwapchainException.h"

namespace Render
{
	RenderTarget::RenderTarget(
		VK::Instance const &instance,
		VK::PhysicalDevice const &physicalDevice,
		VK::Device &device,
		VK::Queue &queue,
		Dev::MemoryAllocator &memoryAllocator,
		Infra::DeferredDeleter &deferredDeleter,
		HINSTANCE const hinstance,
		HWND const hwnd,
		bool const useDepthBuffer,
		bool const useStencilBuffer) :
		__instance			{ instance },
		__physicalDevice	{ physicalDevice },
		__device			{ device },
		__que				{ queue },
		__memoryAllocator	{ memoryAllocator },
		__deferredDeleter	{ deferredDeleter },
		__useDepthBuffer	{ useDepthBuffer },
		__useStencilBuffer	{ useStencilBuffer }
	{
		__pLayerInvalidateListener = Infra::EventListener<Layer *>::bind(
			&RenderTarget::__onLayerInvalidated, this, std::placeholders::_1);

		__pLayerPriorityChangeListener = Infra::EventListener<Layer const *, int, int>::bind(
			&RenderTarget::__onLayerPriorityChanged, this);

		__pLayerNeedRedrawListener = Infra::EventListener<Layer const *>::bind(
			&RenderTarget::__onLayerRedrawNeeded, this);

		__createSurface(hinstance, hwnd);

		__syncSurface();
		__syncSwapchain();

		if (__useDepthBuffer || __useStencilBuffer)
		{
			__resolveDepthStencilProps();
			__syncDepthStencilBuffer();
		}

		__createClearRenderPass();
		__syncClearFramebuffers();

		__pDrawcallCmdBufferCirculator = std::make_unique<Dev::CommandBufferCirculator>(
			__device, __que.getFamilyIndex(),
			VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY, 2U, 30U);

		__pImageAcqSemaphoreCirculator = std::make_unique<Dev::SemaphoreCirculator>(
			__device, VkSemaphoreType::VK_SEMAPHORE_TYPE_BINARY, Constants::DEFERRED_DELETER_QUEUE_SIZE);

		__pCompleteSemaphoreCirculator = std::make_unique<Dev::SemaphoreCirculator>(
			__device, VkSemaphoreType::VK_SEMAPHORE_TYPE_BINARY, Constants::DEFERRED_DELETER_QUEUE_SIZE);

		auto const &extent{ getExtent() };
		__pRendererResourceManager = std::make_unique<RendererResourceManager>(__deferredDeleter);
		__pRendererResourceManager->invalidate(__surfaceFormat.format, extent.width, extent.height);
	}

	RenderTarget::~RenderTarget() noexcept
	{
		__layers.clear();
		__que.vkQueueWaitIdle();

		__pRendererResourceManager = nullptr;

		__pCompleteSemaphoreCirculator = nullptr;
		__pImageAcqSemaphoreCirculator = nullptr;
		__pDrawcallCmdBufferCirculator = nullptr;

		__clearFramebuffers.clear();

		__pDepthStencilImageView = nullptr;
		__pDepthStencilImage = nullptr;

		__swapchainImageViews.clear();
		__swapchainImages.clear();
		__pSwapchain = nullptr;

		__pClearRenderPass = nullptr;
		__pSurface = nullptr;
	}

	void RenderTarget::addLayer(
		Layer *const pLayer)
	{
		if (!(__layers.emplace(pLayer).second))
			throw std::runtime_error{ "The layer is already added." };

		pLayer->getInvalidateEvent() += __pLayerInvalidateListener;
		pLayer->getPriorityChangeEvent() += __pLayerPriorityChangeListener;
		pLayer->getNeedRedrawEvent() += __pLayerNeedRedrawListener;

		__invalidatedLayers.emplace(pLayer);
		__layerSortionInvalidated = true;
		_invalidate();
	}

	void RenderTarget::removeLayer(
		Layer *const pLayer)
	{
		if (!(__layers.erase(pLayer)))
			throw std::runtime_error{ "The layer is not added yet." };

		pLayer->getInvalidateEvent() -= __pLayerInvalidateListener;
		pLayer->getPriorityChangeEvent() -= __pLayerPriorityChangeListener;
		pLayer->getNeedRedrawEvent() -= __pLayerNeedRedrawListener;

		__invalidatedLayers.erase(pLayer);
		__layerSortionInvalidated = true;
		_invalidate();

		__needRedrawEvent.invoke(this);
	}

	void RenderTarget::setClearColor(
		glm::vec4 const &color) noexcept
	{
		std::memcpy(__clearColor.float32, &color, sizeof(color));
		__needRedrawEvent.invoke(this);
	}

	void RenderTarget::setClearDepth(
		float const depth) noexcept
	{
		__clearDepthStencil.depth = depth;
		__needRedrawEvent.invoke(this);
	}

	void RenderTarget::setClearStencil(
		uint32_t const stencil) noexcept
	{
		__clearDepthStencil.stencil = stencil;
		__needRedrawEvent.invoke(this);
	}

	void RenderTarget::sync()
	{
		__que.vkQueueWaitIdle();

		__syncSurface();
		__syncSwapchain();
		
		if (__useDepthBuffer || __useStencilBuffer)
			__syncDepthStencilBuffer();

		__syncClearFramebuffers();

		auto const &extent{ getExtent() };
		__pRendererResourceManager->invalidate(__surfaceFormat.format, extent.width, extent.height);
	}

	RenderTarget::DrawResult RenderTarget::draw()
	{
		auto &imageAcqSemaphore	{ __pImageAcqSemaphoreCirculator->getNext() };
		uint32_t const imageIdx	{ __acquireNextImage(imageAcqSemaphore) };

		auto &completeSemaphore	{ __pCompleteSemaphoreCirculator->getNext() };
		auto &outputAttachment	{ *(__swapchainImageViews[imageIdx]) };
		auto &cmdBuffer			{ __beginNextDrawcallCmdBuffer() };

		__beginSwapchainImage(cmdBuffer, imageIdx);

		for (auto const pLayer : __sortedLayers)
		{
			pLayer->draw(
				cmdBuffer, outputAttachment,
				*__pRendererResourceManager, __renderArea);
		}

		__endSwapchainImage(cmdBuffer, imageIdx);
		cmdBuffer.vkEndCommandBuffer();

		DrawResult retVal{ };
		retVal.pCmdBuffer			= &cmdBuffer;
		retVal.pSwapchain			= __pSwapchain.get();
		retVal.imageIndex			= imageIdx;
		retVal.pImageAcqSemaphore	= &imageAcqSemaphore;
		retVal.pSignalSemaphore		= &completeSemaphore;

		return retVal;
	}

	void RenderTarget::_onValidate()
	{
		for (const auto pLayer : __invalidatedLayers)
			pLayer->validate();

		if (__layerSortionInvalidated)
			__sortLayers();

		__invalidatedLayers.clear();
		__layerSortionInvalidated = false;
	}

	void RenderTarget::__createSurface(
		HINSTANCE const hinstance,
		HWND const hwnd)
	{
		const VkWin32SurfaceCreateInfoKHR createInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR },
			.hinstance		{ hinstance },
			.hwnd			{ hwnd }
		};

		__pSurface = std::make_unique<VK::Surface>(__instance, createInfo);

		__presentModeInfo.sType		= VkStructureType::VK_STRUCTURE_TYPE_SURFACE_PRESENT_MODE_EXT;

		__surfaceInfo.sType			= VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR;
		__surfaceInfo.pNext			= &__presentModeInfo;
		__surfaceInfo.surface		= __pSurface->getHandle();
	}

	void RenderTarget::__syncSurface()
	{
		__verifySurfaceSupport();
		__resolvePresentMode();
		__resolveCapabilities();
		__resolveSurfaceFormat();
		__resolveCompositeAlpha();
	}

	void RenderTarget::__syncSwapchain()
	{
		__swapchainImageViews.clear();
		__swapchainImages.clear();

		auto pOldSwapchain{ std::move(__pSwapchain) };

		if (!(isPresentable()))
			return;

		__createSwapchain(pOldSwapchain.get());
		__enumerateSwapchainImages();
		__createSwapchainImageViews();
	}

	void RenderTarget::__syncDepthStencilBuffer()
	{
		__pDepthStencilImageView = nullptr;
		__pDepthStencilImage = nullptr;

		if (!(isPresentable()))
			return;

		__createDepthStencilImage();
		__createDepthStencilImageView();
	}

	void RenderTarget::__syncClearFramebuffers()
	{
		__clearFramebuffers.clear();

		if (!(isPresentable()))
			return;

		__createClearFramebuffers();
	}

	void RenderTarget::__verifySurfaceSupport()
	{
		VkBool32 supported{ };
		__physicalDevice.vkGetPhysicalDeviceSurfaceSupportKHR(
			__que.getFamilyIndex(), __pSurface->getHandle(), &supported);

		if (!supported)
			throw std::runtime_error{ "The device doesn't support current surface." };
	}

	void RenderTarget::__resolvePresentMode() noexcept
	{
		uint32_t presentModeCount{ };
		__physicalDevice.vkGetPhysicalDeviceSurfacePresentModesKHR(
			__pSurface->getHandle(), &presentModeCount, nullptr);

		std::vector<VkPresentModeKHR> supportedModes;
		supportedModes.resize(presentModeCount);

		__physicalDevice.vkGetPhysicalDeviceSurfacePresentModesKHR(
			__pSurface->getHandle(), &presentModeCount, supportedModes.data());

		__presentMode = VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR;
		for (const auto mode : supportedModes)
		{
			if (mode != VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR)
				continue;

			__presentMode = VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR;
			break;
		}

		__presentModeInfo.presentMode = __presentMode;
	}

	void RenderTarget::__resolveCapabilities()
	{
		__capabilities.sType = VkStructureType::VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_KHR;
		__capabilities.pNext = nullptr;

		auto const result
		{
			__physicalDevice.vkGetPhysicalDeviceSurfaceCapabilities2KHR(
				&__surfaceInfo, &__capabilities)
		};

		if (result != VkResult::VK_SUCCESS)
			throw std::runtime_error{ "Cannot resolve capabilities." };

		__renderArea.offset.x	= 0;
		__renderArea.offset.y	= 0;
		__renderArea.extent		= getExtent();
	}

	void RenderTarget::__resolveSurfaceFormat()
	{
		uint32_t formatCount{ };
		__physicalDevice.vkGetPhysicalDeviceSurfaceFormats2KHR(&__surfaceInfo, &formatCount, nullptr);

		std::vector<VkSurfaceFormat2KHR> formats;
		formats.resize(formatCount);

		for (auto &format : formats)
			format.sType = VkStructureType::VK_STRUCTURE_TYPE_SURFACE_FORMAT_2_KHR;

		__physicalDevice.vkGetPhysicalDeviceSurfaceFormats2KHR(&__surfaceInfo, &formatCount, formats.data());

		__surfaceFormat.format = VkFormat::VK_FORMAT_UNDEFINED;
		for (auto const &format : formats)
		{
			if (
				(format.surfaceFormat.format == VkFormat::VK_FORMAT_B8G8R8A8_SRGB) &&
				(format.surfaceFormat.colorSpace == VkColorSpaceKHR::VK_COLOR_SPACE_SRGB_NONLINEAR_KHR))
			{
				__surfaceFormat = format.surfaceFormat;
				break;
			}
		}

		if (__surfaceFormat.format == VkFormat::VK_FORMAT_UNDEFINED)
			throw std::runtime_error{ "Cannot resolve suitable surface format." };
	}

	void RenderTarget::__resolveCompositeAlpha() noexcept
	{
		auto const &capabilities{ __capabilities.surfaceCapabilities };

		for (int iterBit{ 1 }; iterBit; iterBit <<= 1)
		{
			if (capabilities.supportedCompositeAlpha & iterBit)
			{
				__compositeAlpha = static_cast<VkCompositeAlphaFlagBitsKHR>(iterBit);
				return;
			}
		}
	}

	void RenderTarget::__createSwapchain(
		VK::Swapchain *const pOldSwapchain)
	{
		auto const &capabilities	{ __capabilities.surfaceCapabilities };
		auto const minImageCount	{ std::clamp(3U, capabilities.minImageCount, capabilities.maxImageCount)};

		VkSwapchainCreateInfoKHR const createInfo
		{
			.sType					{ VkStructureType::VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR },
			.surface				{ __pSurface->getHandle() },
			.minImageCount			{ minImageCount },
			.imageFormat			{ __surfaceFormat.format },
			.imageColorSpace		{ __surfaceFormat.colorSpace },
			.imageExtent			{ capabilities.currentExtent },
			.imageArrayLayers		{ 1U },
			.imageUsage				{ VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT },
			.imageSharingMode		{ VkSharingMode::VK_SHARING_MODE_EXCLUSIVE },
			.preTransform			{ capabilities.currentTransform },
			.compositeAlpha			{ __compositeAlpha },
			.presentMode			{ __presentMode },
			.clipped				{ VK_TRUE },
			.oldSwapchain			{ pOldSwapchain ? pOldSwapchain->getHandle() : VK_NULL_HANDLE }
		};

		__pSwapchain = std::make_unique<VK::Swapchain>(__device, createInfo);
	}

	void RenderTarget::__enumerateSwapchainImages()
	{
		uint32_t imageCount{ };
		__device.vkGetSwapchainImagesKHR(__pSwapchain->getHandle(), &imageCount, nullptr);

		if (!imageCount)
			throw std::runtime_error{ "No swapchain images are detected." };

		std::vector<VkImage> imageHandles;
		imageHandles.resize(imageCount);

		__device.vkGetSwapchainImagesKHR(__pSwapchain->getHandle(), &imageCount, imageHandles.data());

		for (auto const handle : imageHandles)
			__swapchainImages.emplace_back(std::make_unique<VK::Image>(handle, __surfaceFormat.format));
	}

	void RenderTarget::__createSwapchainImageViews()
	{
		for (auto const &pImage : __swapchainImages)
		{
			VkImageViewUsageCreateInfo const usageInfo
			{
				.sType	{ VkStructureType::VK_STRUCTURE_TYPE_IMAGE_VIEW_USAGE_CREATE_INFO },
				.usage	{ VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT }
			};

			VkImageViewCreateInfo const createInfo
			{
				.sType					{ VkStructureType::VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO },
				.pNext					{ &usageInfo },
				.image					{ pImage->getHandle() },
				.viewType				{ VkImageViewType::VK_IMAGE_VIEW_TYPE_2D },
				.format					{ __surfaceFormat.format },
				.components				{
					.r					{ VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY },
					.g					{ VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY },
					.b					{ VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY },
					.a					{ VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY }
				},
				.subresourceRange		{
					.aspectMask			{ VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT },
					.baseMipLevel		{ 0U },
					.levelCount			{ 1U },
					.baseArrayLayer		{ 0U },
					.layerCount			{ 1U }
				}
			};

			__swapchainImageViews.emplace_back(std::make_unique<VK::ImageView>(__device, createInfo));
		}
	}

	void RenderTarget::__resolveDepthStencilProps()
	{
		if (__useDepthBuffer && __useStencilBuffer)
		{
			__depthStencilFormat		= VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT;
			__depthStencilImageLayout	= VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			__depthStencilAspectMask	= (
				VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT |
				VkImageAspectFlagBits::VK_IMAGE_ASPECT_STENCIL_BIT
			);
		}
		else if (__useDepthBuffer)
		{
			__depthStencilFormat		= VkFormat::VK_FORMAT_D32_SFLOAT;
			__depthStencilImageLayout	= VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
			__depthStencilAspectMask	= VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT;
		}
		else
		{
			__depthStencilFormat		= VkFormat::VK_FORMAT_S8_UINT;
			__depthStencilImageLayout	= VkImageLayout::VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL;
			__depthStencilAspectMask	= VkImageAspectFlagBits::VK_IMAGE_ASPECT_STENCIL_BIT;
		}

		auto const formatProp{ __physicalDevice.getFormatPropsOf(__depthStencilFormat) };

		if (!(formatProp.optimalTilingFeatures & VkFormatFeatureFlagBits::VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT))
			throw std::runtime_error{ "Cannot resolve the depth stencil format." };
	}

	void RenderTarget::__createDepthStencilImage()
	{
		auto const &extent{ getExtent() };

		VkImageCreateInfo const createInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO },
			.imageType		{ VkImageType::VK_IMAGE_TYPE_2D },
			.format			{ __depthStencilFormat },
			.extent			{ 
				.width		{ extent.width },
				.height		{ extent.height },
				.depth		{ 1U }
			},
			.mipLevels		{ 1U },
			.arrayLayers	{ 1U },
			.samples		{ VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT },
			.tiling			{ VkImageTiling::VK_IMAGE_TILING_OPTIMAL },
			.usage			{ VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT },
			.sharingMode	{ VkSharingMode::VK_SHARING_MODE_EXCLUSIVE }
		};

		__pDepthStencilImage = std::make_unique<Dev::MemoryImage>(
			__device, __memoryAllocator, createInfo,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	}

	void RenderTarget::__createDepthStencilImageView()
	{
		VkImageViewUsageCreateInfo const usageInfo
		{
			.sType	{ VkStructureType::VK_STRUCTURE_TYPE_IMAGE_VIEW_USAGE_CREATE_INFO },
			.usage	{ VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT }
		};

		VkImageViewCreateInfo createInfo
		{
			.sType					{ VkStructureType::VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO },
			.pNext					{ &usageInfo },
			.image					{ __pDepthStencilImage->getHandle() },
			.viewType				{ VkImageViewType::VK_IMAGE_VIEW_TYPE_2D },
			.format					{ __depthStencilFormat },
			.subresourceRange		{
				.aspectMask			{ __depthStencilAspectMask },
				.baseMipLevel		{ 0U },
				.levelCount			{ 1U },
				.baseArrayLayer		{ 0U },
				.layerCount			{ 1U }
			}
		};

		__pDepthStencilImageView = std::make_unique<VK::ImageView>(__device, createInfo);
	}

	void RenderTarget::__createClearRenderPass()
	{
		std::vector<VkAttachmentDescription2> attachmentDescs;

		auto &colorAttachment			{ attachmentDescs.emplace_back() };
		colorAttachment.sType			= VkStructureType::VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
		colorAttachment.format			= __surfaceFormat.format;
		colorAttachment.samples			= VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp			= VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp			= VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp	= VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp	= VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout	= VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout		= VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		if (__useDepthBuffer || __useStencilBuffer)
		{
			auto &depthStencilAttachment			{ attachmentDescs.emplace_back() };
			depthStencilAttachment.sType			= VkStructureType::VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
			depthStencilAttachment.format			= __depthStencilFormat;
			depthStencilAttachment.samples			= VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
			
			depthStencilAttachment.loadOp			= (
				__useDepthBuffer ?
				VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR :
				VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE
			);

			depthStencilAttachment.stencilLoadOp	= (
				__useStencilBuffer ?
				VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR :
				VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE
			);

			depthStencilAttachment.storeOp			= VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depthStencilAttachment.stencilStoreOp	= VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depthStencilAttachment.initialLayout	= VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
			depthStencilAttachment.finalLayout		= __depthStencilImageLayout;
		}

		VkAttachmentReference2 const colorAttachmentRef
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2 },
			.attachment		{ 0U },
			.layout			{ VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
			.aspectMask		{ VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT }
		};

		std::unique_ptr<VkAttachmentReference2> pDepthStencilAttachmentRef;
		if (__useDepthBuffer || __useStencilBuffer)
		{
			pDepthStencilAttachmentRef					= std::make_unique<VkAttachmentReference2>();
			pDepthStencilAttachmentRef->sType			= VkStructureType::VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
			pDepthStencilAttachmentRef->attachment		= 1U;
			pDepthStencilAttachmentRef->layout			= __depthStencilImageLayout;
			pDepthStencilAttachmentRef->aspectMask		= __depthStencilAspectMask;
		};

		VkSubpassDescription2 const subpass
		{
			.sType						{ VkStructureType::VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2 },
			.pipelineBindPoint			{ VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS },
			.colorAttachmentCount		{ 1U },
			.pColorAttachments			{ &colorAttachmentRef },
			.pDepthStencilAttachment	{ pDepthStencilAttachmentRef.get() }
		};

		VkMemoryBarrier2 memoryBarrier
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 },
			.srcStageMask	{ VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT },
			.srcAccessMask	{ VK_ACCESS_2_NONE },
			.dstStageMask	{ VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT },
			.dstAccessMask	{
				VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT |
				VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT
			}
		};

		if (__useDepthBuffer || __useStencilBuffer)
		{
			memoryBarrier.srcStageMask		|= VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT;
			memoryBarrier.srcStageMask		|= VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
			memoryBarrier.srcAccessMask		|= VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			memoryBarrier.dstStageMask		|= VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT;
			memoryBarrier.dstStageMask		|= VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
			memoryBarrier.dstAccessMask		|= VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
			memoryBarrier.dstAccessMask		|= VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		}

		VkSubpassDependency2 const dependency
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2 },
			.pNext			{ &memoryBarrier },
			.srcSubpass		{ VK_SUBPASS_EXTERNAL },
			.dstSubpass		{ 0U }
		};

		VkRenderPassCreateInfo2 const createInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2 },
			.attachmentCount	{ static_cast<uint32_t>(attachmentDescs.size()) },
			.pAttachments		{ attachmentDescs.data() },
			.subpassCount		{ 1U },
			.pSubpasses			{ &subpass },
			.dependencyCount	{ 1U },
			.pDependencies		{ &dependency }
		};

		__pClearRenderPass = std::make_unique<VK::RenderPass>(__device, createInfo);
	}

	void RenderTarget::__createClearFramebuffers()
	{
		auto const &extent{ getExtent() };

		for (auto const &pSwapchainImageView : __swapchainImageViews)
		{
			std::vector<VkImageView> attachments;
			attachments.emplace_back(pSwapchainImageView->getHandle());
			attachments.emplace_back(__pDepthStencilImageView->getHandle());

			VkFramebufferCreateInfo const createInfo
			{
				.sType				{ VkStructureType::VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO },
				.renderPass			{ __pClearRenderPass->getHandle() },
				.attachmentCount	{ static_cast<uint32_t>(attachments.size()) },
				.pAttachments		{ attachments.data() },
				.width				{ extent.width },
				.height				{ extent.height },
				.layers				{ 1U }
			};

			__clearFramebuffers.emplace_back(std::make_unique<VK::Framebuffer>(__device, createInfo));
		}
	}

	uint32_t RenderTarget::__acquireNextImage(
		VK::Semaphore &imageAcqSemaphore)
	{
		uint32_t retVal{ };

		VkAcquireNextImageInfoKHR const imageAcqInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_ACQUIRE_NEXT_IMAGE_INFO_KHR },
			.swapchain		{ __pSwapchain->getHandle() },
			.timeout		{ std::numeric_limits<uint64_t>::max() },
			.semaphore		{ imageAcqSemaphore.getHandle() },
			.deviceMask		{ 1U }
		};

		auto const result{ __device.vkAcquireNextImage2KHR(&imageAcqInfo, &retVal) };
		if (result != VkResult::VK_SUCCESS)
			throw SwapchainException{ "Failed to acquire the swapchain image.", result };

		return retVal;
	}

	VK::CommandBuffer &RenderTarget::__beginNextDrawcallCmdBuffer()
	{
		VK::CommandBuffer &retVal{ __pDrawcallCmdBufferCirculator->getNext() };

		VkCommandBufferBeginInfo const cbBeginInfo
		{
			.sType	{ VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO },
			.flags	{ VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT }
		};

		auto const result{ retVal.vkBeginCommandBuffer(&cbBeginInfo) };
		if (result != VkResult::VK_SUCCESS)
			throw std::runtime_error{ "Cannot begin a drawcall command buffer." };

		return retVal;
	}

	void RenderTarget::__beginSwapchainImage(
		VK::CommandBuffer &cmdBuffer,
		uint32_t const imageIndex)
	{
		std::vector<VkClearValue> clearValues;
		clearValues.emplace_back().color = __clearColor;

		if (__useDepthBuffer || __useStencilBuffer)
			clearValues.emplace_back().depthStencil = __clearDepthStencil;

		VkRenderPassBeginInfo const renderPassBeginInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO },
			.renderPass			{ __pClearRenderPass->getHandle() },
			.framebuffer		{ __clearFramebuffers[imageIndex]->getHandle() },
			.renderArea			{ __renderArea },
			.clearValueCount	{ static_cast<uint32_t>(clearValues.size()) },
			.pClearValues		{ clearValues.data() }
		};

		VkSubpassBeginInfo const subpassBeginInfo
		{
			.sType		{ VkStructureType::VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO },
			.contents	{ VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE }
		};

		VkSubpassEndInfo const subpassEndInfo
		{
			.sType		{ VkStructureType::VK_STRUCTURE_TYPE_SUBPASS_END_INFO }
		};

		cmdBuffer.vkCmdBeginRenderPass2(&renderPassBeginInfo, &subpassBeginInfo);
		cmdBuffer.vkCmdEndRenderPass2(&subpassEndInfo);
	}

	void RenderTarget::__endSwapchainImage(
		VK::CommandBuffer &cmdBuffer,
		uint32_t const imageIndex)
	{
		VkImageMemoryBarrier2 const imageBarrier
		{
			.sType					{ VkStructureType::VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 },
			.srcStageMask			{ VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT },
			.srcAccessMask			{ VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT },
			.dstStageMask			{ VK_PIPELINE_STAGE_2_NONE },
			.dstAccessMask			{ VK_ACCESS_2_NONE },
			.oldLayout				{ VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
			.newLayout				{ VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR },
			.image					{ __swapchainImages[imageIndex]->getHandle() },
			.subresourceRange		{
				.aspectMask			{ VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT },
				.baseMipLevel		{ 0U },
				.levelCount			{ 1U },
				.baseArrayLayer		{ 0U },
				.layerCount			{ 1U }
			}
		};

		VkDependencyInfo const dependencyInfo
		{
			.sType						{ VkStructureType::VK_STRUCTURE_TYPE_DEPENDENCY_INFO },
			.imageMemoryBarrierCount	{ 1U },
			.pImageMemoryBarriers		{ &imageBarrier }
		};

		cmdBuffer.vkCmdPipelineBarrier2(&dependencyInfo);
	}

	void RenderTarget::__sortLayers()
	{
		__sortedLayers.clear();

		for (auto const &pLayer : __layers)
			__sortedLayers.emplace_back(pLayer);

		std::sort(__sortedLayers.begin(), __sortedLayers.end(), [] (auto const lhs, auto const rhs)
		{
			return (lhs->getPriority() < rhs->getPriority());
		});
	}

	void RenderTarget::__onLayerInvalidated(
		Layer *const pLayer) noexcept
	{
		__invalidatedLayers.emplace(pLayer);
		_invalidate();
	}

	void RenderTarget::__onLayerPriorityChanged() noexcept
	{
		__layerSortionInvalidated = true;
		_invalidate();
		__needRedrawEvent.invoke(this);
	}

	void RenderTarget::__onLayerRedrawNeeded() const noexcept
	{
		__needRedrawEvent.invoke(this);
	}
}