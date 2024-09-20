#pragma once

#include "../Render/Renderer.h"

namespace Frx
{
	class ImageRenderer : public Render::Renderer
	{
	public:
		[[nodiscard]]
		virtual bool isValidMaterialPack(
			Render::MaterialPack const &materialPack) const noexcept override;

		[[nodiscard]]
		virtual std::optional<uint32_t> getMaterialSlotOf(
			std::type_index const &materialType) const noexcept;

		[[nodiscard]]
		virtual bool useMaterial() const noexcept;

		[[nodiscard]]
		virtual std::unique_ptr<VK::RenderPass> createRenderPass(
			VkFormat outputFormat) const override;

		[[nodiscard]]
		virtual std::unique_ptr<VK::Framebuffer> createFramebuffer(
			VK::RenderPass &renderPass,
			VK::ImageView &outputAttachment,
			uint32_t outputWidth,
			uint32_t outputHeight) const override;

		[[nodiscard]]
		virtual std::unique_ptr<VK::Pipeline> createPipeline(
			VK::RenderPass &renderPass,
			uint32_t outputWidth,
			uint32_t outputHeight) const override;

	protected:
		static constexpr uint32_t __IMAGE_MATERIAL_SLOT{ 0U };

		std::unique_ptr<VK::ShaderModule> __pVertexShader;
		std::unique_ptr<VK::ShaderModule> __pFragmentShader;

		[[nodiscard]]
		virtual InitResult _onInit() override;

		[[nodiscard]]
		std::shared_ptr<VK::PipelineLayout> __createPipelineLayout() const;
	};
}