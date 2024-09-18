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
		__clearResources();

		__outputFormat		= outputFormat;
		__outputWidth		= outputWidth;
		__outputHeight		= outputHeight;

		__pRendererDestroyListener = Infra::EventListener<Renderer const *>::bind(
			&RendererResourceManager::__onRendererDestroyed, this, std::placeholders::_1);
	}

	VK::RenderPass &RendererResourceManager::getRenderPassOf(
		Renderer const *const pRenderer)
	{
		auto &pResource{ __resources[pRenderer] };
		if (!pResource)
		{
			pRenderer->getDestroyEvent() += __pRendererDestroyListener;
			pResource = std::make_shared<__RendererResource>();
		}

		auto &pRetVal{ pResource->pRenderPass };
		if (!pRetVal)
			pRetVal = pRenderer->createRenderPass(__outputFormat);

		return *pRetVal;
	}

	VK::Framebuffer &RendererResourceManager::getFramebufferOf(
		Renderer const *const pRenderer,
		VK::ImageView &outputAttachment)
	{
		auto &pResource{ __resources[pRenderer] };
		if (!pResource)
		{
			pRenderer->getDestroyEvent() += __pRendererDestroyListener;
			pResource = std::make_shared<__RendererResource>();
		}

		auto &pRetVal{ pResource->framebuffers[&outputAttachment] };
		if (!pRetVal)
		{
			pRetVal = pRenderer->createFramebuffer(
				getRenderPassOf(pRenderer), outputAttachment,
				__outputWidth, __outputHeight);
		}

		return *pRetVal;
	}

	VK::Pipeline &RendererResourceManager::getPipelineOf(
		Renderer const *const pRenderer)
	{
		auto &pResource{ __resources[pRenderer] };
		if (!pResource)
		{
			pRenderer->getDestroyEvent() += __pRendererDestroyListener;
			pResource = std::make_shared<__RendererResource>();
		}

		auto &pRetVal{ pResource->pPipeline };
		if (!pRetVal)
		{
			pRetVal = pRenderer->createPipeline(
				getRenderPassOf(pRenderer),
				__outputWidth, __outputHeight);
		}

		return *pRetVal;
	}

	void RendererResourceManager::__clearResources() noexcept
	{
		for (auto &[_, pResource] : __resources)
			__deferredDeleter.reserve(std::move(pResource));

		__resources.clear();
	}

	void RendererResourceManager::__onRendererDestroyed(
		Renderer const *pRenderer)
	{
		__resources.erase(pRenderer);
	}

	RendererResourceManager::__RendererResource::~__RendererResource() noexcept
	{
		pPipeline = nullptr;
		framebuffers.clear();
		pRenderPass = nullptr;
	}
}