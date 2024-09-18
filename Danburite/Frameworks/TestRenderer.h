#pragma once

#include "../Render/Renderer.h"
#include "../Render/Material.h"

namespace Frx
{
	class TestRenderer : public Render::Renderer
	{
	public:
		[[nodiscard]]
		virtual bool isValidMaterialPack(
			Render::MaterialPack const &materialPack) const noexcept override;

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
		static constexpr uint32_t __TEST_MATERIAL_SLOT{ 0U };

		std::unique_ptr<VK::ShaderModule> __pVertexShader;
		std::unique_ptr<VK::ShaderModule> __pFragmentShader;

		[[nodiscard]]
		virtual InitResult _onInit() override;

		[[nodiscard]]
		std::shared_ptr<VK::PipelineLayout> __createPipelineLayout() const;
	};
}