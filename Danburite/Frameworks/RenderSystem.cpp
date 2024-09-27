#include "RenderSystem.h"
#include "ImageMaterial.h"
#include "SimpleMaterial.h"
#include "TransformMaterial.h"
#include "PhongMaterial.h"
#include <new>

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
			__getRenderEngine().~Engine();
		}).wait();
	}

	std::unique_ptr<Display> RenderSystem::createDisplay(
		HINSTANCE const hinstance,
		HWND const hwnd)
	{
		return std::make_unique<Display>(
			__rcmdExecutor, __getRenderEngine(), hinstance, hwnd);
	}

	void RenderSystem::__createEngine(
		Dev::Context &context,
		VK::PhysicalDevice const &physicalDevice)
	{
		// NOTE: Need to sync with Constants.glsl

		Render::GlobalDescriptorManager::BindingInfo globalDescBindingInfo;
		globalDescBindingInfo.materialBufferLocations[typeid(SimpleMaterial)]		= 0U;
		globalDescBindingInfo.materialBufferLocations[typeid(ImageMaterial)]		= 1U;
		globalDescBindingInfo.materialBufferLocations[typeid(TransformMaterial)]	= 2U;
		globalDescBindingInfo.materialBufferLocations[typeid(PhongMaterial)]		= 3U;

		new (__enginePlaceholder.data()) Render::Engine{ context, physicalDevice, globalDescBindingInfo };
	}

	Render::Engine &RenderSystem::__getRenderEngine() noexcept
	{
		return *(reinterpret_cast<Render::Engine *>(__enginePlaceholder.data()));
	}
}