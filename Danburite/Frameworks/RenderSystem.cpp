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
		__rcmdExecutor.destroy(std::move(__pEngine)).wait();
	}

	std::unique_ptr<Display> RenderSystem::createDisplay(
		HINSTANCE const hinstance,
		HWND const hwnd)
	{
		return std::make_unique<Display>(__rcmdExecutor, __pEngine, hinstance, hwnd);
	}

	std::shared_ptr<Canvas> RenderSystem::createCanvas()
	{
		return std::make_shared<Canvas>(__rcmdExecutor, __pEngine);
	}

	std::shared_ptr<Model> RenderSystem::createModel()
	{
		return std::make_shared<Model>(__rcmdExecutor, __pEngine);
	}

	std::shared_ptr<Drawable> RenderSystem::createDrawable()
	{
		return std::make_shared<Drawable>(__rcmdExecutor);
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

		__pEngine.create(context, physicalDevice, globalDescBindingInfo);
	}
}