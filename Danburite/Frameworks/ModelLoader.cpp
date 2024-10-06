#include "ModelLoader.h"
#include "../System/Env.h"
#include <assimp/postprocess.h>

namespace Frx
{
	void ModelLoader::filterPrimitiveType(
		aiPrimitiveType const type,
		bool const filtered) noexcept
	{
		if (filtered)
			__primitiveTypeFlags |= type;
		else
			__primitiveTypeFlags &= ~type;

		__importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, __primitiveTypeFlags);
	}

	Model::CreateInfo ModelLoader::load(
		std::string_view const &assetPath)
	{
		auto &assetManager	{ Sys::Env::getInstance().getAssetManager() };
		auto const binary	{ assetManager.readBinary(assetPath) };

		auto const pScene
		{
			__importer.ReadFileFromMemory(
				binary.data(), binary.size(),
				aiPostProcessSteps::aiProcess_Triangulate |
				aiPostProcessSteps::aiProcess_FlipUVs |
				aiPostProcessSteps::aiProcess_JoinIdenticalVertices |
				aiPostProcessSteps::aiProcess_GenUVCoords |
				aiPostProcessSteps::aiProcess_OptimizeGraph |
				aiPostProcessSteps::aiProcess_OptimizeMeshes |
				aiPostProcessSteps::aiProcess_SortByPType)
		};

		if (!pScene || (pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE))
			throw std::runtime_error{ __importer.GetErrorString() };

		Model::CreateInfo retVal;

		retVal.materials	= __loadMaterial(pScene->mMaterials, pScene->mNumMaterials);
		retVal.meshes		= __loadMesh(pScene->mMeshes, pScene->mNumMeshes);

		__importer.FreeScene();
		return retVal;
	}

	std::vector<Model::Mesh> ModelLoader::__loadMesh(
		aiMesh const *const *const mMeshes,
		uint32_t const mNumMeshes)
	{
		std::vector<Model::Mesh> retVal;
		retVal.resize(mNumMeshes);

		for (uint32_t meshIt{ }; meshIt < mNumMeshes; ++meshIt)
		{
			auto const pAiMesh	{ mMeshes[meshIt] };
			auto &mesh			{ retVal[meshIt] };

			mesh.vertexCount = pAiMesh->mNumVertices;

			if (pAiMesh->HasPositions())
			{
				auto &posBuffer{ mesh.vertexBuffers[VertexAttribFlagBits::POS] };
				posBuffer.add(pAiMesh->mVertices, pAiMesh->mNumVertices * sizeof(Vertex_P));
			}

			if (pAiMesh->HasTextureCoords(0U))
			{
				auto &uvBuffer				{ mesh.vertexBuffers[VertexAttribFlagBits::UV] };
				auto const pAiTexCoords		{ pAiMesh->mTextureCoords[0] };

				for (uint32_t texCoordIt{ }; texCoordIt < pAiMesh->mNumVertices; ++texCoordIt)
					uvBuffer.add(pAiTexCoords + texCoordIt, sizeof(Vertex_U));
			}

			if (pAiMesh->HasNormals())
			{
				auto &normalBuffer{ mesh.vertexBuffers[VertexAttribFlagBits::NORMAL] };
				normalBuffer.add(pAiMesh->mNormals, pAiMesh->mNumVertices * sizeof(Vertex_N));
			}

			if (pAiMesh->HasVertexColors(0U))
			{
				auto &colorBuffer{ mesh.vertexBuffers[VertexAttribFlagBits::COLOR] };
				colorBuffer.add(pAiMesh->mColors[0], pAiMesh->mNumVertices * sizeof(Vertex_C));
			}

			auto &indexType{ mesh.indexType };
			indexType = VkIndexType::VK_INDEX_TYPE_UINT16;

			std::vector<uint16_t> indices16;
			std::vector<uint32_t> indices32;

			for (uint32_t primitiveIt{ }; primitiveIt < pAiMesh->mNumFaces; ++primitiveIt)
			{
				auto const &aiFace{ pAiMesh->mFaces[primitiveIt] };
				for (uint32_t indexIt{ }; indexIt < aiFace.mNumIndices; ++indexIt)
				{
					uint32_t const index{ aiFace.mIndices[indexIt] };

					if (index <= UINT16_MAX)
						indices16.emplace_back(static_cast<uint16_t>(index));
					else
						indexType = VkIndexType::VK_INDEX_TYPE_UINT32;

					indices32.emplace_back(index);
				}
			}

			mesh.indexCount = static_cast<uint32_t>(indices32.size());

			if (indexType == VkIndexType::VK_INDEX_TYPE_UINT16)
				mesh.indexBuffer.add(indices16.data(), indices16.size() * sizeof(uint16_t));
			else
				mesh.indexBuffer.add(indices32.data(), indices32.size() * sizeof(uint32_t));
		}

		return retVal;
	}

	std::vector<Model::Material> ModelLoader::__loadMaterial(
		aiMaterial const *const *mMaterials,
		uint32_t const mNumMaterials)
	{
		std::vector<Model::Material> retVal;
		retVal.resize(mNumMaterials);

		for (uint32_t materialIt{ }; materialIt < mNumMaterials; ++materialIt)
		{
			auto const pAiMaterial	{ mMaterials[materialIt] };
			auto &material			{ retVal[materialIt] };

			aiColor3D ambient{ 0.0f, 0.0f, 0.0f };
			pAiMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
			material.ambient.r = ambient.r;
			material.ambient.g = ambient.g;
			material.ambient.b = ambient.b;

			aiColor3D diffuse{ 0.0f, 0.0f, 0.0f };
			pAiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
			material.diffuse.r = diffuse.r;
			material.diffuse.g = diffuse.g;
			material.diffuse.b = diffuse.b;

			aiColor3D specular{ 0.0f, 0.0f, 0.0f };
			pAiMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specular);
			material.specular.r = specular.r;
			material.specular.g = specular.g;
			material.specular.b = specular.b;

			aiColor3D emissive{ 0.0f, 0.0f, 0.0f };
			pAiMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, emissive);
			material.emissive.r = emissive.r;
			material.emissive.g = emissive.g;
			material.emissive.b = emissive.b;

			aiColor3D transparent{ 1.0f, 1.0f, 1.0f };
			pAiMaterial->Get(AI_MATKEY_COLOR_TRANSPARENT, transparent);
			material.transparent.r = transparent.r;
			material.transparent.g = transparent.g;
			material.transparent.b = transparent.b;

			aiColor3D reflective{ 0.0f, 0.0f, 0.0f };
			pAiMaterial->Get(AI_MATKEY_COLOR_REFLECTIVE, reflective);
			material.reflective.r = reflective.r;
			material.reflective.g = reflective.g;
			material.reflective.b = reflective.b;

			float reflectivity{ 0.0f };
			pAiMaterial->Get(AI_MATKEY_REFLECTIVITY, reflectivity);
			material.reflectivity = reflectivity;

			bool wireframe{ };
			pAiMaterial->Get(AI_MATKEY_ENABLE_WIREFRAME, wireframe);
			material.wireframe = wireframe;

			bool twoSided{ };
			pAiMaterial->Get(AI_MATKEY_TWOSIDED, twoSided);
			material.twoSided = twoSided;

			aiShadingMode shadingModel{ aiShadingMode::aiShadingMode_Gouraud };
			pAiMaterial->Get(AI_MATKEY_SHADING_MODEL, shadingModel);
			switch (shadingModel)
			{
				case aiShadingMode::aiShadingMode_Flat:
					material.shadingModel = Model::ShadingModel::FLAT;
					break;

				case aiShadingMode::aiShadingMode_Gouraud:
					material.shadingModel = Model::ShadingModel::GOURAUD;
					break;

				case aiShadingMode::aiShadingMode_Phong:
					material.shadingModel = Model::ShadingModel::PHONG;
					break;

				case aiShadingMode::aiShadingMode_Blinn:
					material.shadingModel = Model::ShadingModel::BLINN;
					break;

				case aiShadingMode::aiShadingMode_OrenNayar:
					material.shadingModel = Model::ShadingModel::OREN_NAYAR;
					break;

				case aiShadingMode::aiShadingMode_Minnaert:
					material.shadingModel = Model::ShadingModel::MINNAERT;
					break;

				case aiShadingMode::aiShadingMode_CookTorrance:
					material.shadingModel = Model::ShadingModel::COOK_TORRANCE;
					break;

				case aiShadingMode::aiShadingMode_Unlit:
					material.shadingModel = Model::ShadingModel::UNLIT;
					break;

				case aiShadingMode::aiShadingMode_Fresnel:
					material.shadingModel = Model::ShadingModel::FRESNEL;
					break;

				case aiShadingMode::aiShadingMode_PBR_BRDF:
					material.shadingModel = Model::ShadingModel::PBR_BRDF;
					break;
			}

			aiBlendMode blendMode{ aiBlendMode::aiBlendMode_Default };
			pAiMaterial->Get(AI_MATKEY_BLEND_FUNC, blendMode);
			switch (blendMode)
			{
				case aiBlendMode::aiBlendMode_Default:
					material.blendMode = Model::BlendMode::DEFAULT;
					break;

				case aiBlendMode::aiBlendMode_Additive:
					material.blendMode = Model::BlendMode::ADDITIVE;
					break;
			}

			float opacity{ 1.0f };
			pAiMaterial->Get(AI_MATKEY_OPACITY, opacity);
			material.opacity = opacity;

			float shininess{ 0.0f };
			pAiMaterial->Get(AI_MATKEY_SHININESS, shininess);
			material.shininess = shininess;
			if ((material.shadingModel == Model::ShadingModel::GOURAUD) && (shininess > 0.0f))
				material.shadingModel = Model::ShadingModel::PHONG;

			float shininessStrength{ 1.0f };
			pAiMaterial->Get(AI_MATKEY_SHININESS_STRENGTH, shininessStrength);
			material.shininessStrength = shininessStrength;

			float indexOfRefraction{ 1.0f };
			pAiMaterial->Get(AI_MATKEY_REFRACTI, indexOfRefraction);
			material.indexOfRefraction = indexOfRefraction;

			std::unordered_map<std::string, size_t> texNames;

			for (
				int texTypeIter{ aiTextureType::aiTextureType_NONE };
				texTypeIter < AI_TEXTURE_TYPE_MAX; ++texTypeIter)
			{
				auto const texCount{ pAiMaterial->GetTextureCount(aiTextureType_DIFFUSE) };

				for (uint32_t texIter{ }; texIter < texCount; ++texIter)
				{
					aiString texName{ };
					pAiMaterial->Get(AI_MATKEY_TEXTURE(texTypeIter, texIter), texName);

					int a = 0;
				}
			}
		}

		return retVal;
	}
}