#include "Renderer.h"

namespace Render
{
	void Renderer::init(
		VK::PhysicalDevice &physicalDevice,
		VK::Device &device,
		Infra::DeferredDeleter &deferredDeleter,
		VK::DescriptorSetLayout const &renderTargetDescSetLayout)
	{
		__pPhysicalDevice				= &physicalDevice;
		__pDevice						= &device;
		__pDeferredDeleter				= &deferredDeleter;
		__pRenderTargetDescSetLayout	= &renderTargetDescSetLayout;

		_onInit();
	}

	bool Renderer::isValidMaterialPack(
		MaterialPack const &materialPack) const noexcept
	{
		return false;
	}
}