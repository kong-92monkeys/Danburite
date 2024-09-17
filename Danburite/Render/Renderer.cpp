#include "Renderer.h"
#include "../System/Env.h"
#include "../Device/ShaderIncluder.h"

namespace Render
{
	Renderer::~Renderer() noexcept
	{
		__destroyEvent.invoke(this);
	}

	void Renderer::init(
		VK::PhysicalDevice &physicalDevice,
		VK::Device &device,
		VK::PipelineCache &pipelineCache,
		Infra::DeferredDeleter &deferredDeleter,
		VK::DescriptorSetLayout const &globalDescSetLayout)
	{
		__pPhysicalDevice		= &physicalDevice;
		__pDevice				= &device;
		__pPipelineCache		= &pipelineCache;
		__pDeferredDeleter		= &deferredDeleter;
		__pGlobalDescSetLayout	= &globalDescSetLayout;

		auto initResult{ _onInit() };

		__pPipelineLayout	= std::move(initResult.pPipelineLayout);
	}

	VK::PipelineLayout const &Renderer::getPipelineLayout() const noexcept
	{
		return *__pPipelineLayout;
	}

	bool Renderer::isValidMaterialPack(
		MaterialPack const &materialPack) const noexcept
	{
		return false;
	}

	std::optional<uint32_t> Renderer::getMaterialSlotOf(
		std::type_index const &materialType) const noexcept
	{
		return std::nullopt;
	}

	std::unique_ptr<VK::ShaderModule> Renderer::_createShaderModule(
		std::string_view const &assetPath) const
	{
		const auto code{ __readShaderFile(assetPath) };

		VkShaderModuleCreateInfo const createInfo
		{
			.sType		{ VkStructureType::VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO },
			.codeSize	{ code.size() * sizeof(uint32_t) },
			.pCode		{ code.data() }
		};

		return std::make_unique<VK::ShaderModule>(*__pDevice, createInfo);
	}

	std::vector<uint32_t> Renderer::__readShaderFile(
		std::string_view const &assetPath) const
	{
		auto &assetManager{ Sys::Env::getInstance().getAssetManager() };

		auto const source			{ assetManager.readString(assetPath) };
		auto const compileOptions	{ __makeCopileOptions() };
		
		shaderc::Compiler compiler;
		auto const result
		{
			compiler.CompileGlslToSpv(
				source, shaderc_shader_kind::shaderc_glsl_infer_from_source,
				assetPath.data(), compileOptions)
		};

		if (result.GetCompilationStatus() != shaderc_compilation_status::shaderc_compilation_status_success)
		{
			auto const what
			{
				std::format(
					R"(Error occurred while compiling "{}". msg: "{}")",
					assetPath, result.GetErrorMessage())
			};

			throw std::runtime_error{ what };
		}

		return { result.begin(), result.end() };
	}

	shaderc::CompileOptions Renderer::__makeCopileOptions() noexcept
	{
		shaderc::CompileOptions retVal;

		retVal.SetWarningsAsErrors();
		retVal.SetPreserveBindings(true);

		retVal.SetTargetSpirv(shaderc_spirv_version::shaderc_spirv_version_1_6);
		retVal.SetOptimizationLevel(shaderc_optimization_level::shaderc_optimization_level_performance);

		retVal.SetTargetEnvironment(
			shaderc_target_env::shaderc_target_env_vulkan,
			shaderc_env_version::shaderc_env_version_vulkan_1_3);

		retVal.SetIncluder(std::unique_ptr<shaderc::CompileOptions::IncluderInterface>{ new Dev::ShaderIncluder });

		return retVal;
	}
}