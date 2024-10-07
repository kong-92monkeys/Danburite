#include "AssimpAssetIOSystem.h"
#include "ModelLoader.h"
#include "../System/Env.h"
#include <assimp/postprocess.h>
#include <stdexcept>

namespace Frx
{
	ModelLoader::ModelLoader()
	{
		__importer.SetIOHandler(new AssimpAssetIOSystem);
	}

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
		auto const pScene
		{
			__importer.ReadFile(
				assetPath.data(),
				aiPostProcessSteps::aiProcess_Triangulate |
				aiPostProcessSteps::aiProcess_FlipUVs |
				aiPostProcessSteps::aiProcess_JoinIdenticalVertices |
				aiPostProcessSteps::aiProcess_GenUVCoords |
				aiPostProcessSteps::aiProcess_OptimizeGraph |
				aiPostProcessSteps::aiProcess_OptimizeMeshes |
				aiPostProcessSteps::aiProcess_SortByPType)
		};

		if (!pScene || (pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE))
		{
			auto const errStr{ __importer.GetErrorString() };
			throw std::runtime_error{ errStr };
		}

		auto const assetDir{ std::filesystem::path{ assetPath }.parent_path() };

		Model::CreateInfo retVal;

		__loadTexturesAndMaterials(
			pScene->mMaterials, pScene->mNumMaterials,
			assetDir, retVal.textures, retVal.materials);
		
		__loadMeshes(
			pScene->mMeshes, pScene->mNumMeshes,
			retVal.meshes);

		__importer.FreeScene();

		return retVal;
	}

	void ModelLoader::__loadMeshes(
		aiMesh const *const *const mMeshes,
		uint32_t const mNumMeshes,
		std::vector<Model::Mesh> &outMeshes)
	{
		outMeshes.resize(mNumMeshes);

		for (uint32_t meshIt{ }; meshIt < mNumMeshes; ++meshIt)
		{
			auto const pAiMesh	{ mMeshes[meshIt] };
			auto &mesh			{ outMeshes[meshIt] };

			if (pAiMesh->GetNumUVChannels() > 1)
				throw std::runtime_error{ "Cannot handle UV channels more than 1." };

			if (pAiMesh->GetNumColorChannels() > 1)
				throw std::runtime_error{ "Cannot handle color channels more than 1." };

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
	}

	void ModelLoader::__loadTexturesAndMaterials(
		aiMaterial const *const *mMaterials,
		uint32_t const mNumMaterials,
		std::filesystem::path const &assetDir,
		std::vector<std::unique_ptr<Infra::Bitmap>> &outTextures,
		std::vector<Model::Material> &outMaterials)
	{
		outMaterials.resize(mNumMaterials);

		size_t texCount{ };
		std::unordered_map<std::string, size_t> texNames;

		for (uint32_t materialIt{ }; materialIt < mNumMaterials; ++materialIt)
		{
			auto const pAiMaterial	{ mMaterials[materialIt] };
			auto &material			{ outMaterials[materialIt] };

			// Ambient
			{
				aiColor3D ambient{ 0.0f, 0.0f, 0.0f };
				pAiMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
				material.ambient = __parseAIType(ambient);
			}
			
			// Diffuse
			{
				aiColor3D diffuse{ 0.0f, 0.0f, 0.0f };
				pAiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
				material.diffuse = __parseAIType(diffuse);
			}

			// Specular
			{
				aiColor3D specular{ 0.0f, 0.0f, 0.0f };
				pAiMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specular);
				material.specular = __parseAIType(specular);
			}

			// Emissive
			{
				aiColor3D emissive{ 0.0f, 0.0f, 0.0f };
				pAiMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, emissive);
				material.emissive = __parseAIType(emissive);
			}

			// Transparent
			{
				aiColor3D transparent{ 1.0f, 1.0f, 1.0f };
				pAiMaterial->Get(AI_MATKEY_COLOR_TRANSPARENT, transparent);
				material.transparent = __parseAIType(transparent);
			}

			// Reflective
			{
				aiColor3D reflective{ 0.0f, 0.0f, 0.0f };
				pAiMaterial->Get(AI_MATKEY_COLOR_REFLECTIVE, reflective);
				material.reflective = __parseAIType(reflective);
			}

			// Reflectivity
			{
				float reflectivity{ 0.0f };
				pAiMaterial->Get(AI_MATKEY_REFLECTIVITY, reflectivity);
				material.reflectivity = reflectivity;
			}

			// Wireframe
			{
				bool wireframe{ };
				pAiMaterial->Get(AI_MATKEY_ENABLE_WIREFRAME, wireframe);
				material.wireframe = wireframe;
			}

			// Two-sided
			{
				bool twoSided{ };
				pAiMaterial->Get(AI_MATKEY_TWOSIDED, twoSided);
				material.twoSided = twoSided;
			}

			// Shading model
			{
				aiShadingMode shadingModel{ aiShadingMode::aiShadingMode_Gouraud };
				pAiMaterial->Get(AI_MATKEY_SHADING_MODEL, shadingModel);
				material.shadingModel = __parseAIType(shadingModel);
			}

			// Blend mode
			{
				aiBlendMode blendMode{ aiBlendMode::aiBlendMode_Default };
				pAiMaterial->Get(AI_MATKEY_BLEND_FUNC, blendMode);
				material.blendMode = __parseAIType(blendMode);
			}

			// Opacity
			{
				float opacity{ 1.0f };
				pAiMaterial->Get(AI_MATKEY_OPACITY, opacity);
				material.opacity = opacity;
			}

			// Shininess
			{
				float shininess{ 0.0f };
				pAiMaterial->Get(AI_MATKEY_SHININESS, shininess);
				material.shininess = shininess;
				if ((material.shadingModel == Model::ShadingModel::GOURAUD) && (shininess > 0.0f))
					material.shadingModel = Model::ShadingModel::PHONG;
			}

			// Shininess strength
			{
				float shininessStrength{ 1.0f };
				pAiMaterial->Get(AI_MATKEY_SHININESS_STRENGTH, shininessStrength);
				material.shininessStrength = shininessStrength;
			}

			// Index of refraction
			{
				float indexOfRefraction{ 1.0f };
				pAiMaterial->Get(AI_MATKEY_REFRACTI, indexOfRefraction);
				material.indexOfRefraction = indexOfRefraction;
			}

			// Handling texture infos
			for (
				int texTypeIter{ aiTextureType::aiTextureType_NONE };
				texTypeIter < AI_TEXTURE_TYPE_MAX; ++texTypeIter)
			{
				auto const aiTexType	{ static_cast<aiTextureType>(texTypeIter) };
				auto const texType		{ __parseAIType(aiTexType) };
				auto &texInfos			{ material.textureInfos[texType] };

				auto const texInfoCount{ pAiMaterial->GetTextureCount(aiTexType) };
				texInfos.resize(texInfoCount);

				for (uint32_t texInfoIter{ }; texInfoIter < texInfoCount; ++texInfoIter)
				{
					auto &texInfo{ texInfos[texInfoIter] };

					// Index
					{
						aiString aiTexName{ };
						pAiMaterial->Get(AI_MATKEY_TEXTURE(texTypeIter, texInfoIter), aiTexName);

						std::string const texName{ aiTexName.C_Str() };
						auto const texEmplacement{ texNames.try_emplace(texName, texCount) };

						bool const emplaced{ texEmplacement.second };
						if (emplaced)
							++texCount;

						size_t const texIndex{ texEmplacement.first->second };
						texInfo.index = texIndex;
					}

					// Blend
					{
						float blend{ 1.0f };
						pAiMaterial->Get(AI_MATKEY_TEXBLEND(texTypeIter, texInfoIter), blend);
						texInfo.blend = blend;
					}

					// Operation
					{
						aiTextureOp aiOp{ aiTextureOp::aiTextureOp_Multiply };
						pAiMaterial->Get(AI_MATKEY_TEXOP(texTypeIter, texInfoIter), aiOp);
						texInfo.op = __parseAIType(aiOp);
					}

					// Mapping mode U
					{
						aiTextureMapMode aiMapModeU{ aiTextureMapMode::aiTextureMapMode_Wrap };
						pAiMaterial->Get(AI_MATKEY_MAPPINGMODE_U(texTypeIter, texInfoIter), aiMapModeU);
						texInfo.mapModeU = __parseAIType(aiMapModeU);
					}

					// Mapping mode V
					{
						aiTextureMapMode aiMapModeV{ aiTextureMapMode::aiTextureMapMode_Wrap };
						pAiMaterial->Get(AI_MATKEY_MAPPINGMODE_V(texTypeIter, texInfoIter), aiMapModeV);
						texInfo.mapModeV = __parseAIType(aiMapModeV);
					}

					// Other flags
					{
						aiTextureFlags flags{ };
						pAiMaterial->Get(AI_MATKEY_TEXFLAGS(texTypeIter, texInfoIter), flags);

						texInfo.inverted = (flags & aiTextureFlags::aiTextureFlags_Invert);
						texInfo.useAlpha = (flags &aiTextureFlags::aiTextureFlags_UseAlpha);
					}
				}
			}
		}

		auto &assetManager{ Sys::Env::getInstance().getAssetManager() };

		// Load textures
		outTextures.resize(texCount);
		for (auto const &[texName, texIndex] : texNames)
		{
			auto const texPath		{ (assetDir / texName).string() };
			auto const texData		{ assetManager.readBinary(texPath) };
			outTextures[texIndex]	= std::make_unique<Infra::Bitmap>(texData.data(), texData.size(), 4ULL);
		}
	}
}