#pragma once

#include "../Render/Renderer.h"
#include "../Vulkan/Sampler.h"

namespace Frx
{
	class PhongRenderer : public Render::Renderer
	{
	public:
		static constexpr uint32_t TRANSFORM_MATERIAL_SLOT_IDX	{ 0U };
		static constexpr uint32_t PHONG_MATERIAL_SLOT_IDX		{ 1U };

		PhongRenderer() = default;
		virtual ~PhongRenderer() noexcept override;

		[[nodiscard]]
		virtual bool isValidVertexAttribFlags(
			uint32_t flags) const noexcept override;

		[[nodiscard]]
		virtual bool isValidMaterialPack(
			Render::MaterialPack const &materialPack) const noexcept override;

		[[nodiscard]]
		virtual std::optional<uint32_t> getMaterialSlotOf(
			std::type_index const &materialType) const noexcept;

		[[nodiscard]]
		virtual bool useMaterial() const noexcept;

		[[nodiscard]]
		virtual VkDescriptorSet getDescSet() const noexcept;

		[[nodiscard]]
		virtual void bindVertexInput(
			VK::CommandBuffer &cmdBuffer,
			uint32_t vertexAttribFlags) const override;

		[[nodiscard]]
		virtual std::unique_ptr<VK::RenderPass> createRenderPass(
			VkFormat colorFormat,
			VkFormat depthStencilFormat,
			VkImageLayout depthStencilImageLayout) const override;

		[[nodiscard]]
		virtual std::unique_ptr<VK::Framebuffer> createFramebuffer(
			VK::RenderPass &renderPass,
			VK::ImageView &colorAttachment,
			VK::ImageView *pDepthStencilAttachment,
			uint32_t surfaceWidth,
			uint32_t surfaceHeight) const override;

		[[nodiscard]]
		virtual std::unique_ptr<VK::Pipeline> createPipeline(
			VK::RenderPass &renderPass,
			uint32_t surfaceWidth,
			uint32_t surfaceHeight) const override;

	protected:
		[[nodiscard]]
		virtual InitResult _onInit() override;

	private:
		struct __PushConstants
		{
		public:
			uint32_t vertexAttribFlags{ };
		};

		std::unique_ptr<VK::ShaderModule> __pVertexShader;
		std::unique_ptr<VK::ShaderModule> __pFragmentShader;

		std::shared_ptr<VK::DescriptorSetLayout> __pDescSetLayout;
		std::shared_ptr<VK::DescriptorPool> __pDescPool;
		VkDescriptorSet __hDescSet{ };

		std::shared_ptr<VK::Sampler> __pSampler;

		void __createDescSetLayout();
		void __createDescPool();
		void __allocateDescSet();

		void __createSampler();
		void __updateDescSet();

		[[nodiscard]]
		std::shared_ptr<VK::PipelineLayout> __createPipelineLayout() const;
	};
}