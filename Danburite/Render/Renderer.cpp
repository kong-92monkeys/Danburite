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

	std::optional<uint32_t> Renderer::getMaterialDescLocationOf(
		std::type_index const &materialType) const noexcept
	{
		return std::nullopt;
	}

	std::optional<uint32_t> Renderer::getTextureLUTDescLocationOf(
		std::type_index const &materialType) const noexcept
	{
		return std::nullopt;
	}
}