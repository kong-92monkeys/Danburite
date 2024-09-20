#include "SimpleRenderer.h"
#include "SimpleMaterial.h"
#include "Vertex.h"
#include <array>

namespace Frx
{
	bool SimpleRenderer::isValidMaterialPack(
		Render::MaterialPack const &materialPack) const noexcept
	{
		return materialPack.hasMaterialOf<SimpleMaterial>();
	}

	std::optional<uint32_t> SimpleRenderer::getMaterialSlotOf(
		std::type_index const &materialType) const noexcept
	{
		if (materialType == typeid(SimpleMaterial))
			return __SIMPLE_MATERIAL_SLOT;

		return std::nullopt;
	}

	bool SimpleRenderer::useMaterial() const noexcept
	{
		return true;
	}

	std::unique_ptr<VK::RenderPass> SimpleRenderer::createRenderPass(
		VkFormat const outputFormat) const
	{
		VkAttachmentDescription2 const colorAttachment
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2 },
			.format				{ outputFormat },
			.samples			{ VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT },
			.loadOp				{ VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_LOAD },
			.storeOp			{ VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE },
			.stencilLoadOp		{ VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE },
			.stencilStoreOp		{ VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE },
			.initialLayout		{ VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
			.finalLayout		{ VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }
		};

		VkAttachmentReference2 const colorAttachmentRef
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2 },
			.attachment		{ 0U },
			.layout			{ VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
			.aspectMask		{ VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT }
		};

		VkSubpassDescription2 const subpass
		{
			.sType					{ VkStructureType::VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2 },
			.pipelineBindPoint		{ VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS },
			.colorAttachmentCount	{ 1U },
			.pColorAttachments		{ &colorAttachmentRef }
		};

		VkMemoryBarrier2 const memoryBarrier
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 },
			.srcStageMask	{ VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT },
			.srcAccessMask	{ VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT },
			.dstStageMask	{ VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT },
			.dstAccessMask	{ VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT }
		};

		VkSubpassDependency2 const dependency
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2 },
			.pNext			{ &memoryBarrier },
			.srcSubpass		{ VK_SUBPASS_EXTERNAL },
			.dstSubpass		{ 0U }
		};

		VkRenderPassCreateInfo2 const createInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2 },
			.attachmentCount	{ 1U },
			.pAttachments		{ &colorAttachment },
			.subpassCount		{ 1U },
			.pSubpasses			{ &subpass },
			.dependencyCount	{ 1U },
			.pDependencies		{ &dependency }
		};

		return std::make_unique<VK::RenderPass>(_getDevice(), createInfo);
	}

	std::unique_ptr<VK::Framebuffer> SimpleRenderer::createFramebuffer(
		VK::RenderPass &renderPass,
		VK::ImageView &outputAttachment,
		uint32_t const outputWidth,
		uint32_t const outputHeight) const
	{
		VkFramebufferCreateInfo const createInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO },
			.renderPass			{ renderPass.getHandle() },
			.attachmentCount	{ 1U },
			.pAttachments		{ &(outputAttachment.getHandle()) },
			.width				{ outputWidth },
			.height				{ outputHeight },
			.layers				{ 1U }
		};
		
		return std::make_unique<VK::Framebuffer>(_getDevice(), createInfo);
	}

	std::unique_ptr<VK::Pipeline> SimpleRenderer::createPipeline(
		VK::RenderPass &renderPass,
		uint32_t const outputWidth,
		uint32_t const outputHeight) const
	{
		std::vector<VkPipelineShaderStageCreateInfo> stages;

		auto &vertexShaderInfo	{ stages.emplace_back() };
		vertexShaderInfo.sType	= VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertexShaderInfo.stage	= VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
		vertexShaderInfo.module	= __pVertexShader->getHandle();
		vertexShaderInfo.pName	= "main";

		auto &fragShaderInfo	{ stages.emplace_back() };
		fragShaderInfo.sType	= VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderInfo.stage	= VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderInfo.module	= __pFragmentShader->getHandle();
		fragShaderInfo.pName	= "main";

		std::vector<VkVertexInputBindingDescription> vertexBindingDescs;

		auto &posBindingDesc		{ vertexBindingDescs.emplace_back() };
		posBindingDesc.binding		= VertexAttrib::POS_LOCATION;
		posBindingDesc.stride		= sizeof(Vertex_P);
		posBindingDesc.inputRate	= VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX;

		std::vector<VkVertexInputAttributeDescription> vertexAttribDescs;

		auto &posAttribDesc			{ vertexAttribDescs.emplace_back() };
		posAttribDesc.location		= VertexAttrib::POS_LOCATION;
		posAttribDesc.binding		= VertexAttrib::POS_LOCATION;
		posAttribDesc.format		= VkFormat::VK_FORMAT_R32G32B32_SFLOAT;
		posAttribDesc.offset		= 0U;

		VkPipelineVertexInputStateCreateInfo const vertexInputState
		{
			.sType								{ VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO },
			.vertexBindingDescriptionCount		{ static_cast<uint32_t>(vertexBindingDescs.size()) },
			.pVertexBindingDescriptions			{ vertexBindingDescs.data() },
			.vertexAttributeDescriptionCount	{ static_cast<uint32_t>(vertexAttribDescs.size()) },
			.pVertexAttributeDescriptions		{ vertexAttribDescs.data() }
		};
		
		VkPipelineInputAssemblyStateCreateInfo const inputAssemblyState
		{
			.sType						{ VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO },
			.topology					{ VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST },
			.primitiveRestartEnable		{ VK_FALSE }
		};

		VkViewport const viewports
		{
			.x			{ 0.0f },
			.y			{ 0.0f },
			.width		{ static_cast<float>(outputWidth) },
			.height		{ static_cast<float>(outputHeight) },
			.minDepth	{ 0.0f },
			.maxDepth	{ 1.0f }
		};

		VkRect2D const scissors
		{
			.offset		{
				.x		{ 0 },
				.y		{ 0 }
			},
			.extent		{
				.width	{ outputWidth },
				.height	{ outputHeight }
			}
		};

		VkPipelineViewportStateCreateInfo const viewportState
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO },
			.viewportCount	{ 1U },
			.pViewports		{ &viewports },
			.scissorCount	{ 1U },
			.pScissors		{ &scissors }
		};

		VkPipelineRasterizationStateCreateInfo const rasterizationState
		{
			.sType						{ VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO },
			.depthClampEnable			{ VK_FALSE },
			.rasterizerDiscardEnable	{ VK_FALSE },
			.polygonMode				{ VkPolygonMode::VK_POLYGON_MODE_FILL },
			.cullMode					{ VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT },
			.frontFace					{ VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE },
			.depthBiasEnable			{ VK_FALSE },
			.lineWidth					{ 1.0f }
		};

		VkPipelineMultisampleStateCreateInfo const multisampleState
		{
			.sType						{ VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO },
			.rasterizationSamples		{ VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT },
			.sampleShadingEnable		{ VK_FALSE },
			.minSampleShading			{ 1.0f },
			.pSampleMask				{ nullptr },
			.alphaToCoverageEnable		{ VK_FALSE },
			.alphaToOneEnable			{ VK_FALSE }
		};

		VkPipelineColorBlendAttachmentState const blendAttachment
		{
			.blendEnable			{ VK_FALSE },
			.colorWriteMask			{
				VkColorComponentFlagBits::VK_COLOR_COMPONENT_R_BIT |
				VkColorComponentFlagBits::VK_COLOR_COMPONENT_G_BIT |
				VkColorComponentFlagBits::VK_COLOR_COMPONENT_B_BIT |
				VkColorComponentFlagBits::VK_COLOR_COMPONENT_A_BIT
			}
		};

		VkPipelineColorBlendStateCreateInfo const colorBlendState
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO },
			.logicOpEnable		{ VK_FALSE },
			.attachmentCount	{ 1U },
			.pAttachments		{ &blendAttachment }
		};

		VkGraphicsPipelineCreateInfo const createInfo
		{
			.sType						{ VkStructureType::VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO },
			.stageCount					{ static_cast<uint32_t>(stages.size()) },
			.pStages					{ stages.data() },
			.pVertexInputState			{ &vertexInputState },
			.pInputAssemblyState		{ &inputAssemblyState },
			.pTessellationState			{ nullptr },
			.pViewportState				{ &viewportState },
			.pRasterizationState		{ &rasterizationState },
			.pMultisampleState			{ &multisampleState },
			.pDepthStencilState			{ nullptr },
			.pColorBlendState			{ &colorBlendState },
			.pDynamicState				{ nullptr },
			.layout						{ getPipelineLayout().getHandle() },
			.renderPass					{ renderPass.getHandle() },
			.subpass					{ 0U },
			.basePipelineHandle			{ VK_NULL_HANDLE },
			.basePipelineIndex			{ -1 }
		};

		return std::make_unique<VK::Pipeline>(_getDevice(), _getPipelineCache().getHandle(), createInfo);
	}

	Render::Renderer::InitResult SimpleRenderer::_onInit()
	{
		__pVertexShader		= _createShaderModule("Shaders/Simple.vert");
		__pFragmentShader	= _createShaderModule("Shaders/Simple.frag");

		Render::Renderer::InitResult retVal;
		retVal.pPipelineLayout	= __createPipelineLayout();

		return retVal;
	}

	std::shared_ptr<VK::PipelineLayout> SimpleRenderer::__createPipelineLayout() const
	{
		auto const &globalDescManager{ _getGlobalDescriptorManager() };

		std::vector<VkDescriptorSetLayout> setLayouts;

		// MATERIALS_DESC_SET_LOCATION
		setLayouts.emplace_back(globalDescManager.getMaterialsDescSetLayout().getHandle());

		// SAMPLED_IMAGES_DESC_SET_LOCATION
		setLayouts.emplace_back(globalDescManager.getSampledImagesDescSetLayout().getHandle());

		// SUB_LAYER_DESC_SET_LOCATION
		setLayouts.emplace_back(_getSubLayerDescSetLayout().getHandle());

		VkPipelineLayoutCreateInfo const createInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO },
			.setLayoutCount		{ static_cast<uint32_t>(setLayouts.size()) },
			.pSetLayouts		{ setLayouts.data() }
		};

		return std::make_shared<VK::PipelineLayout>(_getDevice(), createInfo);
	}
}