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