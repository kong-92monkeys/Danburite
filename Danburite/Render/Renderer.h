#pragma once

#include "../Vulkan/PhysicalDevice.h"
#include "../Vulkan/Device.h"
#include "../Vulkan/DescriptorSetLayout.h"
#include "../Infra/DeferredDeleter.h"
#include "Material.h"
#include <optional>

namespace Render
{
	class Renderer : public Infra::Unique
	{
	public:
		Renderer() = default;
		virtual ~Renderer() noexcept override = default;

		void init(
			VK::PhysicalDevice &physicalDevice,
			VK::Device &device,
			Infra::DeferredDeleter &deferredDeleter,
			VK::DescriptorSetLayout const &renderTargetDescSetLayout);

		[[nodiscard]]
		virtual bool isValidMaterialPack(
			MaterialPack const &materialPack) const noexcept;

		[[nodiscard]]
		virtual std::optional<uint32_t> getMaterialSlotIndexOf(
			std::type_index const &materialType) const noexcept;

	protected:
		virtual void _onInit() = 0;

	private:
		VK::PhysicalDevice *__pPhysicalDevice{ };
		VK::Device *__pDevice{ };
		Infra::DeferredDeleter *__pDeferredDeleter{ };
		VK::DescriptorSetLayout const *__pRenderTargetDescSetLayout{ };


	};
}
