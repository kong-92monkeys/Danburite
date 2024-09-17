#include "RendererResourceManager.h"

namespace Render
{
	RendererResourceManager::RendererResourceManager(
		Infra::DeferredDeleter &deferredDeleter) noexcept :
		__deferredDeleter	{ deferredDeleter }
	{}

	void RendererResourceManager::invalidate(
		VkFormat const outputFormat,
		uint32_t const outputWidth,
		uint32_t const outputHeight)
	{
		__outputFormat		= outputFormat;
		__outputWidth		= outputWidth;
		__outputHeight		= outputHeight;

		__resources.clear();

		__pRendererDestroyListener = Infra::EventListener<Renderer const *>::bind(
			&RendererResourceManager::__onRendererDestroyed, this, std::placeholders::_1);
	}

	VK::RenderPass &RendererResourceManager::getRenderPassOf(
		Renderer const &renderer)
	{
		if (!(__resources.contains(&renderer)))
			renderer.getDestroyEvent() += __pRendererDestroyListener;

		auto &pRetVal{ __resources[&renderer].pRenderPass };
		if (!pRetVal)
			pRetVal = renderer.createRenderPass(__outputFormat);

		return *pRetVal;
	}

	VK::Framebuffer &RendererResourceManager::getFramebufferOf(
		Renderer const &renderer,
		VK::ImageView &outputAttachment)
	{
		if (!(__resources.contains(&renderer)))
			renderer.getDestroyEvent() += __pRendererDestroyListener;

		auto &pRetVal{ __resources[&renderer].framebuffers[&outputAttachment] };
		if (!pRetVal)
		{
			pRetVal = renderer.createFramebuffer(
				getRenderPassOf(renderer), outputAttachment,
				__outputWidth, __outputHeight);
		}

		return *pRetVal;
	}

	VK::Pipeline &RendererResourceManager::getPipelineOf(
		Renderer const &renderer)
	{
		if (!(__resources.contains(&renderer)))
			renderer.getDestroyEvent() += __pRendererDestroyListener;

		auto &pRetVal{ __resources[&renderer].pPipeline };
		if (!pRetVal)
		{
			pRetVal = renderer.createPipeline(
				getRenderPassOf(renderer),
				__outputWidth, __outputHeight);
		}

		return *pRetVal;
	}

	void RendererResourceManager::__onRendererDestroyed(
		Renderer const *pRenderer)
	{
		__resources.erase(pRenderer);
	}
}