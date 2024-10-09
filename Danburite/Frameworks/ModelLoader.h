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
		static constexpr glm::vec3 __parseAIType(
			aiColor3D const &value) noexcept;

		[[nodiscard]]
		static constexpr RendererType __parseAIType(
			aiShadingMode value) noexcept;

		[[nodiscard]]
		static constexpr Model::BlendMode __parseAIType(
			aiBlendMode value) noexcept;

		[[nodiscard]]
		static constexpr Model::TextureType __parseAIType(
			aiTextureType value) noexcept;

		[[nodiscard]]
		static constexpr Model::TextureOp __parseAIType(
			aiTextureOp value) noexcept;

		[[nodiscard]]
		static constexpr Model::TextureMapMode __parseAIType(
			aiTextureMapMode value) noexcept;

		[[nodiscard]]
		static glm::mat4 __parseAIType(
			aiMatrix4x4 const &value) noexcept;
	};

	constexpr glm::vec3 ModelLoader::__parseAIType(
		aiColor3D const &value) noexcept
	{
		return { value[0], value[1], value[2] };
	}

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

	constexpr Model::BlendMode ModelLoader::__parseAIType(
		aiBlendMode const value) noexcept
	{
		switch (value)
		{
			case aiBlendMode::aiBlendMode_Default:
				return Model::BlendMode::DEFAULT;

			case aiBlendMode::aiBlendMode_Additive:
				return Model::BlendMode::ADDITIVE;
		}

		return Model::BlendMode::DEFAULT;
	}

	constexpr Model::TextureType ModelLoader::__parseAIType(
		aiTextureType const value) noexcept
	{
		switch (value)
		{
			case aiTextureType::aiTextureType_NONE:
				return Model::TextureType::NONE;

			case aiTextureType::aiTextureType_DIFFUSE:
				return Model::TextureType::DIFFUSE;

			case aiTextureType::aiTextureType_SPECULAR:
				return Model::TextureType::SPECULAR;

			case aiTextureType::aiTextureType_AMBIENT:
				return Model::TextureType::AMBIENT;

			case aiTextureType::aiTextureType_EMISSIVE:
				return Model::TextureType::EMISSIVE;

			case aiTextureType::aiTextureType_HEIGHT:
				return Model::TextureType::HEIGHT;

			case aiTextureType::aiTextureType_NORMALS:
				return Model::TextureType::NORMALS;

			case aiTextureType::aiTextureType_SHININESS:
				return Model::TextureType::SHININESS;

			case aiTextureType::aiTextureType_OPACITY:
				return Model::TextureType::OPACITY;

			case aiTextureType::aiTextureType_DISPLACEMENT:
				return Model::TextureType::DISPLACEMENT;

			case aiTextureType::aiTextureType_LIGHTMAP:
				return Model::TextureType::LIGHTMAP;

			case aiTextureType::aiTextureType_REFLECTION:
				return Model::TextureType::REFLECTION;

			case aiTextureType::aiTextureType_BASE_COLOR:
				return Model::TextureType::BASE_COLOR;

			case aiTextureType::aiTextureType_NORMAL_CAMERA:
				return Model::TextureType::NORMAL_CAMERA;

			case aiTextureType::aiTextureType_EMISSION_COLOR:
				return Model::TextureType::EMISSION_COLOR;

			case aiTextureType::aiTextureType_METALNESS:
				return Model::TextureType::METALNESS;

			case aiTextureType::aiTextureType_DIFFUSE_ROUGHNESS:
				return Model::TextureType::DIFFUSE_ROUGHNESS;

			case aiTextureType::aiTextureType_AMBIENT_OCCLUSION:
				return Model::TextureType::AMBIENT_OCCLUSION;

			case aiTextureType::aiTextureType_UNKNOWN:
				return Model::TextureType::UNKNOWN;

			case aiTextureType::aiTextureType_SHEEN:
				return Model::TextureType::SHEEN;

			case aiTextureType::aiTextureType_CLEARCOAT:
				return Model::TextureType::CLEARCOAT;

			case aiTextureType::aiTextureType_TRANSMISSION:
				return Model::TextureType::TRANSMISSION;

			case aiTextureType::aiTextureType_MAYA_BASE:
				return Model::TextureType::MAYA_BASE;

			case aiTextureType::aiTextureType_MAYA_SPECULAR:
				return Model::TextureType::MAYA_SPECULAR;

			case aiTextureType::aiTextureType_MAYA_SPECULAR_COLOR:
				return Model::TextureType::MAYA_SPECULAR_COLOR;

			case aiTextureType::aiTextureType_MAYA_SPECULAR_ROUGHNESS:
				return Model::TextureType::MAYA_SPECULAR_ROUGHNESS;
		}

		return Model::TextureType::UNKNOWN;
	}

	constexpr Model::TextureOp ModelLoader::__parseAIType(
		aiTextureOp const value) noexcept
	{
		switch (value)
		{
			case aiTextureOp::aiTextureOp_Multiply:
				return Model::TextureOp::MULTIPLY;

			case aiTextureOp::aiTextureOp_Add:
				return Model::TextureOp::ADD;

			case aiTextureOp::aiTextureOp_Subtract:
				return Model::TextureOp::SUBTRACT;

			case aiTextureOp::aiTextureOp_Divide:
				return Model::TextureOp::DIVIDE;

			case aiTextureOp::aiTextureOp_SmoothAdd:
				return Model::TextureOp::SMOOTH_ADD;

			case aiTextureOp::aiTextureOp_SignedAdd:
				return Model::TextureOp::SIGNED_ADD;
		}

		return Model::TextureOp::MULTIPLY;
	}

	constexpr Model::TextureMapMode ModelLoader::__parseAIType(
		aiTextureMapMode const value) noexcept
	{
		switch (value)
		{
			case aiTextureMapMode::aiTextureMapMode_Wrap:
				return Model::TextureMapMode::WRAP;

			case aiTextureMapMode::aiTextureMapMode_Clamp:
				return Model::TextureMapMode::CLAMP;

			case aiTextureMapMode::aiTextureMapMode_Decal:
				return Model::TextureMapMode::DECAL;

			case aiTextureMapMode::aiTextureMapMode_Mirror:
				return Model::TextureMapMode::MIRROR;
		}

		return Model::TextureMapMode::WRAP;
	}
}