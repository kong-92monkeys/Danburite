#pragma once

#include <assimp/Importer.hpp>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <filesystem>
#include "Model.h"

namespace Frx
{
	class ModelLoader : public Infra::Unique
	{
	public:
		ModelLoader();

		void filterPrimitiveType(
			aiPrimitiveType type,
			bool filtered) noexcept;

		[[nodiscard]]
		Model::CreateInfo load(
			std::string_view const &assetPath);

	private:
		Assimp::Importer __importer;
		int __primitiveTypeFlags{ };

		[[nodiscard]]
		static void __loadTexturesAndMaterials(
			aiMaterial const *const *pAiMaterials,
			uint32_t materialCount,
			std::filesystem::path const &assetDir,
			std::vector<std::shared_ptr<Infra::Bitmap>> &outTextures,
			std::vector<Model::MaterialInfo> &outMaterials);

		[[nodiscard]]
		static void __loadMeshes(
			aiMesh const *const *pAiMeshes,
			uint32_t meshCount,
			std::vector<Model::MeshInfo> &outMeshes);

		[[nodiscard]]
		static void __loadDrawInfosAndNodes(
			aiNode const *pAiRootNode,
			aiMesh const *const *pAiMeshes,
			std::vector<Model::MeshInfo> const &outMeshes,
			std::vector<Model::DrawInfo> &outDrawInfos,
			std::vector<Model::NodeInfo> &outNodes);

		[[nodiscard]]
		static constexpr RendererType __parseAIType(
			aiShadingMode value) noexcept;

		[[nodiscard]]
		static constexpr AlphaBlendOp __parseAIType(
			aiBlendMode value) noexcept;

		[[nodiscard]]
		static constexpr TextureType __parseAIType(
			aiTextureType value) noexcept;

		[[nodiscard]]
		static constexpr TextureBlendOp __parseAIType(
			aiTextureOp value) noexcept;

		[[nodiscard]]
		static constexpr TextureMapMode __parseAIType(
			aiTextureMapMode value) noexcept;

		[[nodiscard]]
		static glm::mat4 __parseAIType(
			aiMatrix4x4 const &value) noexcept;
	};

	constexpr RendererType ModelLoader::__parseAIType(
		aiShadingMode const shadingModel) noexcept
	{
		switch (shadingModel)
		{
			case aiShadingMode::aiShadingMode_Flat:
				return RendererType::FLAT;

			case aiShadingMode::aiShadingMode_Gouraud:
				return RendererType::GOURAUD;

			case aiShadingMode::aiShadingMode_Phong:
				return RendererType::PHONG;

			case aiShadingMode::aiShadingMode_Blinn:
				return RendererType::BLINN;

			case aiShadingMode::aiShadingMode_OrenNayar:
				return RendererType::OREN_NAYAR;

			case aiShadingMode::aiShadingMode_Minnaert:
				return RendererType::MINNAERT;

			case aiShadingMode::aiShadingMode_CookTorrance:
				return RendererType::COOK_TORRANCE;

			case aiShadingMode::aiShadingMode_Unlit:
				return RendererType::UNLIT;

			case aiShadingMode::aiShadingMode_Fresnel:
				return RendererType::FRESNEL;

			case aiShadingMode::aiShadingMode_PBR_BRDF:
				return RendererType::PBR_BRDF;
		}

		return RendererType::GOURAUD;
	}

	constexpr AlphaBlendOp ModelLoader::__parseAIType(
		aiBlendMode const value) noexcept
	{
		switch (value)
		{
			case aiBlendMode::aiBlendMode_Default:
				return AlphaBlendOp::DEFAULT;

			case aiBlendMode::aiBlendMode_Additive:
				return AlphaBlendOp::ADDITIVE;
		}

		return AlphaBlendOp::DEFAULT;
	}

	constexpr TextureType ModelLoader::__parseAIType(
		aiTextureType const value) noexcept
	{
		switch (value)
		{
			case aiTextureType::aiTextureType_NONE:
				return TextureType::NONE;

			case aiTextureType::aiTextureType_DIFFUSE:
				return TextureType::DIFFUSE;

			case aiTextureType::aiTextureType_SPECULAR:
				return TextureType::SPECULAR;

			case aiTextureType::aiTextureType_AMBIENT:
				return TextureType::AMBIENT;

			case aiTextureType::aiTextureType_EMISSIVE:
				return TextureType::EMISSIVE;

			case aiTextureType::aiTextureType_HEIGHT:
				return TextureType::HEIGHT;

			case aiTextureType::aiTextureType_NORMALS:
				return TextureType::NORMALS;

			case aiTextureType::aiTextureType_SHININESS:
				return TextureType::SHININESS;

			case aiTextureType::aiTextureType_OPACITY:
				return TextureType::OPACITY;

			case aiTextureType::aiTextureType_DISPLACEMENT:
				return TextureType::DISPLACEMENT;

			case aiTextureType::aiTextureType_LIGHTMAP:
				return TextureType::LIGHTMAP;

			case aiTextureType::aiTextureType_REFLECTION:
				return TextureType::REFLECTION;

			case aiTextureType::aiTextureType_BASE_COLOR:
				return TextureType::BASE_COLOR;

			case aiTextureType::aiTextureType_NORMAL_CAMERA:
				return TextureType::NORMAL_CAMERA;

			case aiTextureType::aiTextureType_EMISSION_COLOR:
				return TextureType::EMISSION_COLOR;

			case aiTextureType::aiTextureType_METALNESS:
				return TextureType::METALNESS;

			case aiTextureType::aiTextureType_DIFFUSE_ROUGHNESS:
				return TextureType::DIFFUSE_ROUGHNESS;

			case aiTextureType::aiTextureType_AMBIENT_OCCLUSION:
				return TextureType::AMBIENT_OCCLUSION;

			case aiTextureType::aiTextureType_UNKNOWN:
				return TextureType::UNKNOWN;

			case aiTextureType::aiTextureType_SHEEN:
				return TextureType::SHEEN;

			case aiTextureType::aiTextureType_CLEARCOAT:
				return TextureType::CLEARCOAT;

			case aiTextureType::aiTextureType_TRANSMISSION:
				return TextureType::TRANSMISSION;

			case aiTextureType::aiTextureType_MAYA_BASE:
				return TextureType::MAYA_BASE;

			case aiTextureType::aiTextureType_MAYA_SPECULAR:
				return TextureType::MAYA_SPECULAR;

			case aiTextureType::aiTextureType_MAYA_SPECULAR_COLOR:
				return TextureType::MAYA_SPECULAR_COLOR;

			case aiTextureType::aiTextureType_MAYA_SPECULAR_ROUGHNESS:
				return TextureType::MAYA_SPECULAR_ROUGHNESS;
		}

		return TextureType::UNKNOWN;
	}

	constexpr TextureBlendOp ModelLoader::__parseAIType(
		aiTextureOp const value) noexcept
	{
		switch (value)
		{
			case aiTextureOp::aiTextureOp_Multiply:
				return TextureBlendOp::MULTIPLY;

			case aiTextureOp::aiTextureOp_Add:
				return TextureBlendOp::ADD;

			case aiTextureOp::aiTextureOp_Subtract:
				return TextureBlendOp::SUBTRACT;

			case aiTextureOp::aiTextureOp_Divide:
				return TextureBlendOp::DIVIDE;

			case aiTextureOp::aiTextureOp_SmoothAdd:
				return TextureBlendOp::SMOOTH_ADD;

			case aiTextureOp::aiTextureOp_SignedAdd:
				return TextureBlendOp::SIGNED_ADD;
		}

		return TextureBlendOp::MULTIPLY;
	}

	constexpr TextureMapMode ModelLoader::__parseAIType(
		aiTextureMapMode const value) noexcept
	{
		switch (value)
		{
			case aiTextureMapMode::aiTextureMapMode_Wrap:
				return TextureMapMode::WRAP;

			case aiTextureMapMode::aiTextureMapMode_Clamp:
				return TextureMapMode::CLAMP;

			case aiTextureMapMode::aiTextureMapMode_Decal:
				return TextureMapMode::DECAL;

			case aiTextureMapMode::aiTextureMapMode_Mirror:
				return TextureMapMode::MIRROR;
		}

		return TextureMapMode::WRAP;
	}
}