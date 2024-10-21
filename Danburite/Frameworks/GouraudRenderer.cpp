#include "GouraudRenderer.h"
#include "TransformMaterial.h"
#include "PhongMaterial.h"
#include "Constants.h"
#include "VertexAttribute.h"
#include <array>

namespace Frx
{
	GouraudRenderer::~GouraudRenderer() noexcept
	{
		auto &deferredDeleter{ _getDeferredDeleter() };

		deferredDeleter.reserve(std::move(__pDescPool));
		deferredDeleter.reserve(std::move(__pDescSetLayout));
	}

	bool GouraudRenderer::isValidVertexAttribFlags(
		uint32_t const flags) const noexcept
	{
		return hasFlagBits(
			static_cast<VertexAttribFlags>(flags),
			VertexAttribFlagBits::POS | VertexAttribFlagBits::NORMAL);
	}

	bool GouraudRenderer::isValidMaterialPack(
		Render::MaterialPack const &materialPack) const noexcept
	{
		if (!(materialPack.hasValidMaterialOf<TransformMaterial>()))
			return false;

		if (!(materialPack.hasValidMaterialOf<PhongMaterial>()))
			return false;

		return true;
	}

	std::optional<uint32_t> GouraudRenderer::getMaterialSlotOf(
		std::type_index const &materialType) const noexcept
	{
		if (materialType == typeid(TransformMaterial))
			return TRANSFORM_MATERIAL_SLOT_IDX;

		if (materialType == typeid(PhongMaterial))
			return PHONG_MATERIAL_SLOT_IDX;

		return std::nullopt;
	}

	bool GouraudRenderer::useMaterial() const noexcept
	{
		return true;
	}

	void GouraudRenderer::bindVertexInput(
		VK::CommandBuffer &cmdBuffer,
		uint32_t const vertexAttribFlags) const
	{
		std::vector<VkVertexInputBindingDescription2EXT> bindingDescs;
		std::vector<VkVertexInputAttributeDescription2EXT> attribDescs;

		if (vertexAttribFlags & VertexAttribFlagBits::POS)
		{
			auto &posBindingDesc		{ bindingDescs.emplace_back() };
			posBindingDesc.sType		= VkStructureType::VK_STRUCTURE_TYPE_VERTEX_INPUT_BINDING_DESCRIPTION_2_EXT;
			posBindingDesc.binding		= VertexAttrib::POS_LOCATION;
			posBindingDesc.stride		= VERTEX_ATTRIB_INFOS[VertexAttrib::POS_LOCATION].memSize;
			posBindingDesc.inputRate	= VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX;
			posBindingDesc.divisor		= 1U;

			auto &posAttribDesc			{ attribDescs.emplace_back() };
			posAttribDesc.sType			= VkStructureType::VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT;
			posAttribDesc.location		= VertexAttrib::POS_LOCATION;
			posAttribDesc.binding		= VertexAttrib::POS_LOCATION;
			posAttribDesc.format		= VERTEX_ATTRIB_INFOS[VertexAttrib::POS_LOCATION].format;
			posAttribDesc.offset		= 0U;
		}

		if (vertexAttribFlags & VertexAttribFlagBits::NORMAL)
		{
			auto &normalBindingDesc			{ bindingDescs.emplace_back() };
			normalBindingDesc.sType			= VkStructureType::VK_STRUCTURE_TYPE_VERTEX_INPUT_BINDING_DESCRIPTION_2_EXT;
			normalBindingDesc.binding		= VertexAttrib::NORMAL_LOCATION;
			normalBindingDesc.stride		= VERTEX_ATTRIB_INFOS[VertexAttrib::NORMAL_LOCATION].memSize;
			normalBindingDesc.inputRate		= VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX;
			normalBindingDesc.divisor		= 1U;

			auto &normalAttribDesc			{ attribDescs.emplace_back() };
			normalAttribDesc.sType			= VkStructureType::VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT;
			normalAttribDesc.location		= VertexAttrib::NORMAL_LOCATION;
			normalAttribDesc.binding		= VertexAttrib::NORMAL_LOCATION;
			normalAttribDesc.format			= VERTEX_ATTRIB_INFOS[VertexAttrib::NORMAL_LOCATION].format;
			normalAttribDesc.offset			= 0U;
		}

		if (vertexAttribFlags & VertexAttribFlagBits::COLOR)
		{
			auto &colorBindingDesc		{ bindingDescs.emplace_back() };
			colorBindingDesc.sType		= VkStructureType::VK_STRUCTURE_TYPE_VERTEX_INPUT_BINDING_DESCRIPTION_2_EXT;
			colorBindingDesc.binding	= VertexAttrib::COLOR_LOCATION;
			colorBindingDesc.stride		= VERTEX_ATTRIB_INFOS[VertexAttrib::COLOR_LOCATION].memSize;
			colorBindingDesc.inputRate	= VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX;
			colorBindingDesc.divisor	= 1U;

			auto &colorAttribDesc		{ attribDescs.emplace_back() };
			colorAttribDesc.sType		= VkStructureType::VK_STRUCTURE_TYPE_VERTEX_INPUT_ATTRIBUTE_DESCRIPTION_2_EXT;
			colorAttribDesc.location	= VertexAttrib::COLOR_LOCATION;
			colorAttribDesc.binding		= VertexAttrib::COLOR_LOCATION;
			colorAttribDesc.format		= VERTEX_ATTRIB_INFOS[VertexAttrib::COLOR_LOCATION].format;
			colorAttribDesc.offset		= 0U;
		}

		cmdBuffer.vkCmdSetVertexInputEXT(
			static_cast<uint32_t>(bindingDescs.size()), bindingDescs.data(),
			static_cast<uint32_t>(attribDescs.size()), attribDescs.data());

		__PushConstants pushConstants;
		pushConstants.vertexAttribFlags = vertexAttribFlags;

		cmdBuffer.vkCmdPushConstants(
			getPipelineLayout().getHandle(),
			VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT,
			0U, sizeof(__PushConstants), &pushConstants);
	}

	std::unique_ptr<VK::RenderPass> GouraudRenderer::createRenderPass(
		VkFormat const colorFormat,
		VkFormat const depthStencilFormat,
		VkImageLayout const depthStencilImageLayout) const
	{
		std::vector<VkAttachmentDescription2> attachmentDescs;

		auto &colorAttachment			{ attachmentDescs.emplace_back() };
		colorAttachment.sType			= VkStructureType::VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
		colorAttachment.format			= colorFormat;
		colorAttachment.samples			= VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp			= VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_LOAD;
		colorAttachment.storeOp			= VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp	= VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp	= VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout	= VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		colorAttachment.finalLayout		= VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		auto &depthAttachment			{ attachmentDescs.emplace_back() };
		depthAttachment.sType			= VkStructureType::VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
		depthAttachment.format			= depthStencilFormat;
		depthAttachment.samples			= VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp			= VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_LOAD;
		depthAttachment.storeOp			= VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;
		depthAttachment.stencilLoadOp	= VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp	= VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout	= depthStencilImageLayout;
		depthAttachment.finalLayout		= depthStencilImageLayout;

		VkAttachmentReference2 const colorAttachmentRef
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2 },
			.attachment		{ 0U },
			.layout			{ VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
			.aspectMask		{ VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT }
		};

		VkAttachmentReference2 const depthAttachmentRef
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2 },
			.attachment		{ 1U },
			.layout			{ depthStencilImageLayout },
			.aspectMask		{ VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT }
		};

		VkSubpassDescription2 const subpass
		{
			.sType						{ VkStructureType::VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2 },
			.pipelineBindPoint			{ VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS },
			.colorAttachmentCount		{ 1U },
			.pColorAttachments			{ &colorAttachmentRef },
			.pDepthStencilAttachment	{ &depthAttachmentRef }
		};

		VkMemoryBarrier2 const memoryBarrier
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 },
			.srcStageMask	{
				VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT |
				VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT |
				VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT
			},
			.srcAccessMask	{
				VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT |
				VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
			},
			.dstStageMask	{
				VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT |
				VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT
			},
			.dstAccessMask	{
				VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT |
				VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
				VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
			}
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
			.attachmentCount	{ static_cast<uint32_t>(attachmentDescs.size()) },
			.pAttachments		{ attachmentDescs.data() },
			.subpassCount		{ 1U },
			.pSubpasses			{ &subpass },
			.dependencyCount	{ 1U },
			.pDependencies		{ &dependency }
		};

		return std::make_unique<VK::RenderPass>(_getDevice(), createInfo);
	}

	std::unique_ptr<VK::Framebuffer> GouraudRenderer::createFramebuffer(
		VK::RenderPass &renderPass,
		VK::ImageView &colorAttachment,
		VK::ImageView *const pDepthStencilAttachment,
		uint32_t const surfaceWidth,
		uint32_t const surfaceHeight) const
	{
		std::vector<VkImageView> attachments;
		attachments.emplace_back(colorAttachment.getHandle());

		if (pDepthStencilAttachment)
			attachments.emplace_back(pDepthStencilAttachment->getHandle());

		VkFramebufferCreateInfo const createInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO },
			.renderPass			{ renderPass.getHandle() },
			.attachmentCount	{ static_cast<uint32_t>(attachments.size()) },
			.pAttachments		{ attachments.data() },
			.width				{ surfaceWidth },
			.height				{ surfaceHeight },
			.layers				{ 1U }
		};
		
		return std::make_unique<VK::Framebuffer>(_getDevice(), createInfo);
	}

	std::unique_ptr<VK::Pipeline> GouraudRenderer::createPipeline(
		VK::RenderPass &renderPass,
		uint32_t const surfaceWidth,
		uint32_t const surfaceHeight) const
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
			.width		{ static_cast<float>(surfaceWidth) },
			.height		{ static_cast<float>(surfaceHeight) },
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
				.width	{ surfaceWidth },
				.height	{ surfaceHeight }
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

		VkPipelineDepthStencilStateCreateInfo const depthStencilState
		{
			.sType					{ VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO },
			.depthTestEnable		{ VK_TRUE },
			.depthWriteEnable		{ VK_TRUE },
			.depthCompareOp			{ VkCompareOp::VK_COMPARE_OP_GREATER },
			.depthBoundsTestEnable	{ VK_FALSE },
			.stencilTestEnable		{ VK_FALSE }
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

		static constexpr std::array dynamicStates
		{
			VkDynamicState::VK_DYNAMIC_STATE_VERTEX_INPUT_EXT
		};

		VkPipelineDynamicStateCreateInfo const dynamicStateInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO },
			.dynamicStateCount	{ static_cast<uint32_t>(dynamicStates.size()) },
			.pDynamicStates		{ dynamicStates.data() }
		};

		VkGraphicsPipelineCreateInfo const createInfo
		{
			.sType						{ VkStructureType::VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO },
			.stageCount					{ static_cast<uint32_t>(stages.size()) },
			.pStages					{ stages.data() },
			.pVertexInputState			{ nullptr },
			.pInputAssemblyState		{ &inputAssemblyState },
			.pTessellationState			{ nullptr },
			.pViewportState				{ &viewportState },
			.pRasterizationState		{ &rasterizationState },
			.pMultisampleState			{ &multisampleState },
			.pDepthStencilState			{ &depthStencilState },
			.pColorBlendState			{ &colorBlendState },
			.pDynamicState				{ &dynamicStateInfo },
			.layout						{ getPipelineLayout().getHandle() },
			.renderPass					{ renderPass.getHandle() },
			.subpass					{ 0U },
			.basePipelineHandle			{ VK_NULL_HANDLE },
			.basePipelineIndex			{ -1 }
		};

		return std::make_unique<VK::Pipeline>(_getDevice(), _getPipelineCache().getHandle(), createInfo);
	}

	Render::Renderer::InitResult GouraudRenderer::_onInit()
	{
		__pVertexShader		= _createShaderModule("Shaders/Gouraud.vert");
		__pFragmentShader	= _createShaderModule("Shaders/Gouraud.frag");

		__createDescSetLayout();

		Render::Renderer::InitResult retVal;
		retVal.pPipelineLayout	= __createPipelineLayout();

		return retVal;
	}

	void GouraudRenderer::__createDescSetLayout()
	{
		std::vector<VkDescriptorBindingFlags> bindingFlags;
		std::vector<VkDescriptorSetLayoutBinding> bindings;

		VkDescriptorSetLayoutBindingFlagsCreateInfo const flagInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO },
			.bindingCount	{ static_cast<uint32_t>(bindingFlags.size()) },
			.pBindingFlags	{ bindingFlags.data() }
		};

		VkDescriptorSetLayoutCreateInfo const createInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO },
			.pNext			{ &flagInfo },
			.bindingCount	{ static_cast<uint32_t>(bindings.size()) },
			.pBindings		{ bindings.data() }
		};

		__pDescSetLayout = std::make_shared<VK::DescriptorSetLayout>(_getDevice(), createInfo);
	}

	std::shared_ptr<VK::PipelineLayout> GouraudRenderer::__createPipelineLayout() const
	{
		auto const &globalDescManager{ _getGlobalDescriptorManager() };

		std::array const setLayouts
		{
			// GLOBAL_DESC_SET_LOCATION
			globalDescManager.getGlobalDescSetLayout().getHandle(),

			// MATERIALS_DESC_SET_LOCATION
			globalDescManager.getMaterialsDescSetLayout().getHandle(),
			
			// SAMPLED_IMAGES_DESC_SET_LOCATION
			globalDescManager.getSampledImagesDescSetLayout().getHandle(),

			// SUB_LAYER_DESC_SET_LOCATION
			_getSubLayerDescSetLayout().getHandle(),
			
			// RENDERER_DESC_SET_LOCATION
			__pDescSetLayout->getHandle()
		};

		VkPushConstantRange const pushConstantRange
		{
			.stageFlags		{ VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT },
			.offset			{ 0U },
			.size			{ sizeof(__PushConstants) }
		};

		VkPipelineLayoutCreateInfo const createInfo
		{
			.sType						{ VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO },
			.setLayoutCount				{ static_cast<uint32_t>(setLayouts.size()) },
			.pSetLayouts				{ setLayouts.data() },
			.pushConstantRangeCount		{ 1U },
			.pPushConstantRanges		{ &pushConstantRange }
		};

		return std::make_shared<VK::PipelineLayout>(_getDevice(), createInfo);
	}
}