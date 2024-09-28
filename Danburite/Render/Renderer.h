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
#include "GlobalDescriptorManager.h"
#include <optional>
#include <shaderc/shaderc.hpp>

namespace Render
{
	class Renderer : public Infra::Unique
	{
	public:
		Renderer() = default;
		virtual ~Renderer() noexcept override;

		void init(
			VK::PhysicalDevice const &physicalDevice,
			VK::Device &device,
			VK::PipelineCache &pipelineCache,
			Infra::DeferredDeleter &deferredDeleter,
			Dev::DescriptorUpdater &descriptorUpdater,
			GlobalDescriptorManager const &globalDescriptorManager,
			VK::DescriptorSetLayout const &subLayerDescSetLayout);

		[[nodiscard]]
		constexpr int getPriority() const noexcept;
		void setPriority(
			int priority);

		[[nodiscard]]
		VK::PipelineLayout const &getPipelineLayout() const noexcept;

		[[nodiscard]]
		virtual bool isValidMaterialPack(
			MaterialPack const &materialPack) const noexcept;

		[[nodiscard]]
		virtual std::optional<uint32_t> getMaterialSlotOf(
			std::type_index const &materialType) const noexcept;

		[[nodiscard]]
		virtual bool useMaterial() const noexcept;

		[[nodiscard]]
		virtual VkDescriptorSet getDescSet() const noexcept;

		[[nodiscard]]
		virtual std::unique_ptr<VK::RenderPass> createRenderPass(
			VkFormat colorFormat,
			VkFormat depthStencilFormat,
			VkImageLayout depthStencilImageLayout) const = 0;

		[[nodiscard]]
		virtual std::unique_ptr<VK::Framebuffer> createFramebuffer(
			VK::RenderPass &renderPass,
			VK::ImageView &colorAttachment,
			VK::ImageView *pDepthStencilAttachment,
			uint32_t surfaceWidth,
			uint32_t surfaceHeight) const = 0;

		[[nodiscard]]
		virtual std::unique_ptr<VK::Pipeline> createPipeline(
			VK::RenderPass &renderPass,
			uint32_t surfaceWidth,
			uint32_t surfaceHeight) const = 0;

		[[nodiscard]]
		constexpr Infra::EventView<Renderer const *, int, int> &
			getPriorityChangeEvent() const noexcept;

		[[nodiscard]]
		constexpr Infra::EventView<Renderer const *> &getDestroyEvent() const noexcept;

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
		constexpr VK::PhysicalDevice const &_getPhysicalDevice() const noexcept;

		[[nodiscard]]
		constexpr VK::Device &_getDevice() const noexcept;

		[[nodiscard]]
		constexpr VK::PipelineCache &_getPipelineCache() const noexcept;

		[[nodiscard]]
		constexpr Infra::DeferredDeleter &_getDeferredDeleter() const noexcept;

		[[nodiscard]]
		constexpr Dev::DescriptorUpdater &_getDescriptorUpdater() const noexcept;

		[[nodiscard]]
		constexpr GlobalDescriptorManager const &_getGlobalDescriptorManager() const noexcept;

		[[nodiscard]]
		constexpr VK::DescriptorSetLayout const &_getSubLayerDescSetLayout() const noexcept;

	private:
		VK::PhysicalDevice const *__pPhysicalDevice{ };
		VK::Device *__pDevice{ };
		VK::PipelineCache *__pPipelineCache{ };
		Infra::DeferredDeleter *__pDeferredDeleter{ };
		Dev::DescriptorUpdater *__pDescriptorUpdater{ };
		GlobalDescriptorManager const *__pGlobalDescriptorManager{ };
		VK::DescriptorSetLayout const *__pSubLayerDescSetLayout{ };

		int __priority{ };

		std::shared_ptr<VK::PipelineLayout> __pPipelineLayout;

		mutable Infra::Event<Renderer const *, int, int> __priorityChangeEvent;
		mutable Infra::Event<Renderer const *> __destroyEvent;

		[[nodiscard]]
		std::vector<uint32_t> __readShaderFile(
			std::string_view const &assetPath) const;

		[[nodiscard]]
		static shaderc::CompileOptions __makeCopileOptions() noexcept;
	};

	constexpr int Renderer::getPriority() const noexcept
	{
		return __priority;
	}

	constexpr Infra::EventView<Renderer const *, int, int> &
		Renderer::getPriorityChangeEvent() const noexcept
	{
		return __priorityChangeEvent;
	}

	constexpr Infra::EventView<Renderer const *> &Renderer::getDestroyEvent() const noexcept
	{
		return __destroyEvent;
	}

	constexpr VK::PhysicalDevice const &Renderer::_getPhysicalDevice() const noexcept
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

	constexpr Dev::DescriptorUpdater &Renderer::_getDescriptorUpdater() const noexcept
	{
		return *__pDescriptorUpdater;
	}

	constexpr GlobalDescriptorManager const &Renderer::_getGlobalDescriptorManager() const noexcept
	{
		return *__pGlobalDescriptorManager;
	}

	constexpr VK::DescriptorSetLayout const &Renderer::_getSubLayerDescSetLayout() const noexcept
	{
		return *__pSubLayerDescSetLayout;
	}
}
