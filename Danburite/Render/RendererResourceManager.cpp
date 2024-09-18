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
		Renderer const &renderer)
	{
		auto &pResource{ __resources[&renderer] };
		if (!pResource)
		{
			renderer.getDestroyEvent() += __pRendererDestroyListener;
			pResource = std::make_shared<__RendererResource>();
		}

		auto &pRetVal{ pResource->pRenderPass };
		if (!pRetVal)
			pRetVal = renderer.createRenderPass(__outputFormat);

		return *pRetVal;
	}

	VK::Framebuffer &RendererResourceManager::getFramebufferOf(
		Renderer const &renderer,
		VK::ImageView &outputAttachment)
	{
		auto &pResource{ __resources[&renderer] };
		if (!pResource)
		{
			renderer.getDestroyEvent() += __pRendererDestroyListener;
			pResource = std::make_shared<__RendererResource>();
		}

		auto &pRetVal{ pResource->framebuffers[&outputAttachment] };
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
		auto &pResource{ __resources[&renderer] };
		if (!pResource)
		{
			renderer.getDestroyEvent() += __pRendererDestroyListener;
			pResource = std::make_shared<__RendererResource>();
		}

		auto &pRetVal{ pResource->pPipeline };
		if (!pRetVal)
		{
			pRetVal = renderer.createPipeline(
				getRenderPassOf(renderer),
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