#include "RenderSystem.h"
#include "ImageMaterial.h"
#include "SimpleMaterial.h"

namespace Frx
{
	RenderSystem::RenderSystem(
		Dev::Context &context,
		VK::PhysicalDevice const &physicalDevice)
	{
		__rcmdExecutor.run([this, &context, &physicalDevice]
		{
			__createEngine(context, physicalDevice);
		}).wait();
	}

	RenderSystem::~RenderSystem() noexcept
	{
		__rcmdExecutor.run([this]
		{
			__pEngine = nullptr;
		}).wait();
	}

	Placeholder<Render::RenderTarget> RenderSystem::createRenderTarget(
		HINSTANCE const hinstance,
		HWND const hwnd)
	{
		auto const pProm	{ new std::promise<Render::RenderTarget *> };
		auto fut			{ pProm->get_future() };

		__rcmdExecutor.silentRun([this, pProm, hinstance, hwnd]
		{
			pProm->set_value(__pEngine->createRenderTarget(hinstance, hwnd));
			delete pProm;
		});

		return { __rcmdExecutor, std::move(fut) };
	}

	Placeholder<Render::Layer> RenderSystem::createLayer()
	{
		auto const pProm	{ new std::promise<Render::Layer *> };
		auto fut			{ pProm->get_future() };

		__rcmdExecutor.silentRun([this, pProm]
		{
			pProm->set_value(__pEngine->createLayer());
			delete pProm;
		});

		return { __rcmdExecutor, std::move(fut) };
	}

	Placeholder<Render::Mesh> RenderSystem::createMesh()
	{
		auto const pProm	{ new std::promise<Render::Mesh *> };
		auto fut			{ pProm->get_future() };

		__rcmdExecutor.silentRun([this, pProm]
		{
			pProm->set_value(__pEngine->createMesh());
			delete pProm;
		});

		return { __rcmdExecutor, std::move(fut) };
	}

	Placeholder<Render::Texture> RenderSystem::createTexture(
		Render::Texture::ImageCreateInfo const &imageCreateInfo,
		Render::Texture::ImageViewCreateInfo const &imageViewCreateInfo)
	{
		auto const pProm	{ new std::promise<Render::Texture *> };
		auto fut			{ pProm->get_future() };

		__rcmdExecutor.silentRun([this, pProm, imageCreateInfo, imageViewCreateInfo]
		{
			pProm->set_value(__pEngine->createTexture(imageCreateInfo, imageViewCreateInfo));
			delete pProm;
		});

		return { __rcmdExecutor, std::move(fut) };
	}

	std::shared_ptr<SceneObject> RenderSystem::createSceneObject()
	{
		return std::make_shared<SceneObject>(__rcmdExecutor);
	}

	void RenderSystem::__createEngine(
		Dev::Context &context,
		VK::PhysicalDevice const &physicalDevice)
	{
		Render::GlobalDescriptorManager::BindingInfo globalDescBindingInfo;
		globalDescBindingInfo.materialBufferLocations[typeid(SimpleMaterial)]	= 0U;
		globalDescBindingInfo.materialBufferLocations[typeid(ImageMaterial)]	= 1U;

		__pEngine = std::make_unique<Render::Engine>(context, physicalDevice, globalDescBindingInfo);
	}
}