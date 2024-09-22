#include "RenderSystem.h"
#include "ImageMaterial.h"
#include "SimpleMaterial.h"

namespace Frx
{
	RenderSystem::RenderSystem(
		Dev::Context &context,
		VK::PhysicalDevice const &physicalDevice)
	{
		__rcmdExecutor.silentRun([this, &context, &physicalDevice]
		{
			__createEngine(context, physicalDevice);
		});

		__rcmdExecutor.waitIdle();
	}

	RenderSystem::~RenderSystem() noexcept
	{
		__rcmdExecutor.silentRun([this]
		{
			__pEngine.reset();
		});

		__rcmdExecutor.waitIdle();
	}

	void RenderSystem::__createEngine(
		Dev::Context &context,
		VK::PhysicalDevice const &physicalDevice)
	{
		Render::GlobalDescriptorManager::BindingInfo globalDescBindingInfo;
		globalDescBindingInfo.materialBufferLocations[typeid(SimpleMaterial)]	= 0U;
		globalDescBindingInfo.materialBufferLocations[typeid(ImageMaterial)]	= 1U;

		__pEngine.instantiate(context, physicalDevice, globalDescBindingInfo);
	}
}