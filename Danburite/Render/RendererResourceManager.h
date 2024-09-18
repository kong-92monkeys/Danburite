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
			VkFormat outputFormat,
			uint32_t outputWidth,
			uint32_t outputHeight);

		[[nodiscard]]
		VK::RenderPass &getRenderPassOf(
			Renderer const &renderer);

		[[nodiscard]]
		VK::Framebuffer &getFramebufferOf(
			Renderer const &renderer,
			VK::ImageView &outputAttachment);

		[[nodiscard]]
		VK::Pipeline &getPipelineOf(
			Renderer const &renderer);

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

		VkFormat __outputFormat{ };
		uint32_t __outputWidth{ };
		uint32_t __outputHeight{ };

		std::unordered_map<Renderer const *, std::shared_ptr<__RendererResource>> __resources;

		Infra::EventListenerPtr<Renderer const *> __pRendererDestroyListener;

		void __clearResources() noexcept;

		void __onRendererDestroyed(
			Renderer const *pRenderer);
	};
}
