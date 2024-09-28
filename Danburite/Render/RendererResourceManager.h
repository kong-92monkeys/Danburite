#pragma once

#include "Renderer.h"

namespace Render
{
	class RendererResourceManager : public Infra::Unique
	{
	public:
		RendererResourceManager(
			Infra::DeferredDeleter &deferredDeleter) noexcept;

		void invalidate(
			VkFormat colorFormat,
			VkFormat depthStencilFormat,
			VkImageLayout depthStencilImageLayout,
			uint32_t surfaceWidth,
			uint32_t surfaceHeight);

		[[nodiscard]]
		VK::RenderPass &getRenderPassOf(
			Renderer const *pRenderer);

		[[nodiscard]]
		VK::Framebuffer &getFramebufferOf(
			Renderer const *pRenderer,
			VK::ImageView &colorAttachment,
			VK::ImageView *pDepthStencilAttachment);

		[[nodiscard]]
		VK::Pipeline &getPipelineOf(
			Renderer const *pRenderer);

	private:
		class __RendererResource
		{
		public:
			~__RendererResource() noexcept;

			std::unique_ptr<VK::RenderPass> pRenderPass;
			std::unordered_map<VK::ImageView *, std::unique_ptr<VK::Framebuffer>> framebuffers;
			std::unique_ptr<VK::Pipeline> pPipeline;
		};

		Infra::DeferredDeleter &__deferredDeleter;

		VkFormat __colorFormat{ };
		VkFormat __depthStencilFormat{ };
		VkImageLayout __depthStencilImageLayout{ };

		uint32_t __outputWidth{ };
		uint32_t __outputHeight{ };

		std::unordered_map<Renderer const *, std::shared_ptr<__RendererResource>> __resources;

		Infra::EventListenerPtr<Renderer const *> __pRendererDestroyListener;

		void __clearResources() noexcept;

		void __onRendererDestroyed(
			Renderer const *pRenderer);
	};
}
