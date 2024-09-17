#pragma once

#include "../Infra/DeferredDeleter.h"
#include "../Vulkan/PhysicalDevice.h"
#include "../Vulkan/Device.h"
#include "../Vulkan/DescriptorSetLayout.h"
#include "../Vulkan/PipelineLayout.h"
#include "../Vulkan/Pipeline.h"
#include "../Vulkan/PipelineCache.h"
#include "../Vulkan/RenderPass.h"
#include "../Vulkan/Framebuffer.h"
#include "../Vulkan/CommandBuffer.h"
#include "../Vulkan/ImageView.h"
#include "../Vulkan/ShaderModule.h"
#include "Material.h"
#include <optional>
#include <shaderc/shaderc.hpp>

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
			VK::PipelineCache &pipelineCache,
			Infra::DeferredDeleter &deferredDeleter,
			VK::DescriptorSetLayout const &globalDescSetLayout);

		[[nodiscard]]
		VK::PipelineLayout const &getPipelineLayout() const noexcept;

		[[nodiscard]]
		virtual bool isValidMaterialPack(
			MaterialPack const &materialPack) const noexcept;

		[[nodiscard]]
		virtual std::optional<uint32_t> getMaterialSlotOf(
			std::type_index const &materialType) const noexcept;

		[[nodiscard]]
		virtual std::unique_ptr<VK::RenderPass> createRenderPass(
			VkFormat outputFormat) const = 0;

		[[nodiscard]]
		virtual std::unique_ptr<VK::Framebuffer> createFramebuffer(
			VK::RenderPass &renderPass,
			VK::ImageView &outputAttachment,
			uint32_t outputWidth,
			uint32_t outputHeight) const = 0;

		[[nodiscard]]
		virtual std::unique_ptr<VK::Pipeline> createPipeline(
			VK::RenderPass &renderPass,
			uint32_t outputWidth,
			uint32_t outputHeight) const = 0;

	protected:
		struct InitResult
		{
		public:
			std::shared_ptr<VK::PipelineLayout> pPipelineLayout;
		};

		[[nodiscard]]
		virtual InitResult _onInit() = 0;

		[[nodiscard]]
		std::unique_ptr<VK::ShaderModule> _createShaderModule(
			std::string_view const &assetPath) const;

		[[nodiscard]]
		constexpr VK::PhysicalDevice &_getPhysicalDevice() const noexcept;

		[[nodiscard]]
		constexpr VK::Device &_getDevice() const noexcept;

		[[nodiscard]]
		constexpr VK::PipelineCache &_getPipelineCache() const noexcept;

		[[nodiscard]]
		constexpr Infra::DeferredDeleter &_getDeferredDeleter() const noexcept;

		[[nodiscard]]
		constexpr VK::DescriptorSetLayout const &_getGlobalDescSetLayout() const noexcept;

	private:
		VK::PhysicalDevice *__pPhysicalDevice{ };
		VK::Device *__pDevice{ };
		VK::PipelineCache *__pPipelineCache{ };
		Infra::DeferredDeleter *__pDeferredDeleter{ };
		VK::DescriptorSetLayout const *__pGlobalDescSetLayout{ };

		std::shared_ptr<VK::PipelineLayout> __pPipelineLayout;
		std::shared_ptr<VK::Pipeline> __pPipeline;

		[[nodiscard]]
		std::vector<uint32_t> __readShaderFile(
			std::string_view const &assetPath) const;

		[[nodiscard]]
		static shaderc::CompileOptions __makeCopileOptions() noexcept;
	};

	constexpr VK::PhysicalDevice &Renderer::_getPhysicalDevice() const noexcept
	{
		return *__pPhysicalDevice;
	}

	constexpr VK::Device &Renderer::_getDevice() const noexcept
	{
		return *__pDevice;
	}

	constexpr VK::PipelineCache &Renderer::_getPipelineCache() const noexcept
	{
		return *__pPipelineCache;
	}

	constexpr Infra::DeferredDeleter &Renderer::_getDeferredDeleter() const noexcept
	{
		return *__pDeferredDeleter;
	}

	constexpr VK::DescriptorSetLayout const &Renderer::_getGlobalDescSetLayout() const noexcept
	{
		return *__pGlobalDescSetLayout;
	}
}
