#include "AssimpAssetIOSystem.h"
#include "ModelLoader.h"
#include "../System/Env.h"
#include <assimp/postprocess.h>
#include <stdexcept>
#include <optional>
#include <future>

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
				aiPostProcessSteps::aiProcess_JoinIdenticalVertices |
				aiPostProcessSteps::aiProcess_RemoveRedundantMaterials |
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

		std::vector<std::future<void>> jobs;

		jobs.emplace_back(
			std::async(std::launch::async, [pScene, assetDir, &retVal]
		{
			__loadTexturesAndMaterials(
				pScene->mMaterials, pScene->mNumMaterials,
				assetDir, retVal.textures, retVal.materials);
		}));

		jobs.emplace_back(
			std::async(std::launch::async, [pScene, &retVal]
		{
			// TODO: mesh merging (if needed)
			__loadMeshes(
				pScene->mMeshes, pScene->mNumMeshes,
				retVal.meshes);

			__loadDrawInfosAndNodes(
				pScene->mRootNode, pScene->mMeshes,
				retVal.meshes, retVal.drawInfos, retVal.nodes);
		}));

		for (auto const &job : jobs)
			job.wait();

		__importer.FreeScene();

		return retVal;
	}

	void ModelLoader::__loadTexturesAndMaterials(
		aiMaterial const *const *pAiMaterials,
		uint32_t const materialCount,
		std::filesystem::path const &assetDir,
		std::vector<std::shared_ptr<Infra::Bitmap>> &outTextures,
		std::vector<Model::MaterialInfo> &outMaterials)
	{
		outMaterials.resize(materialCount);

		uint32_t texCount{ };
		std::unordered_map<std::string, uint32_t> texNames;

		for (uint32_t materialIt{ }; materialIt < materialCount; ++materialIt)
		{
			auto const pAiMaterial	{ pAiMaterials[materialIt] };
			auto &material			{ outMaterials[materialIt] };

			// Ambient
			{
				aiColor3D ambient{ 0.0f, 0.0f, 0.0f };
				pAiMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
				material.ambient.r = ambient.r;
				material.ambient.g = ambient.g;
				material.ambient.b = ambient.b;
			}
			
			// Diffuse
			{
				aiColor3D diffuse{ 0.0f, 0.0f, 0.0f };
				pAiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
				material.diffuse.r = diffuse.r;
				material.diffuse.g = diffuse.g;
				material.diffuse.b = diffuse.b;
			}

			// Specular
			{
				aiColor3D specular{ 0.0f, 0.0f, 0.0f };
				pAiMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specular);
				material.specular.r = specular.r;
				material.specular.g = specular.g;
				material.specular.b = specular.b;
			}

			// Emissive
			{
				aiColor3D emissive{ 0.0f, 0.0f, 0.0f };
				pAiMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, emissive);
				material.emissive.r = emissive.r;
				material.emissive.g = emissive.g;
				material.emissive.b = emissive.b;
			}

			// Transparent
			{
				aiColor3D transparent{ 1.0f, 1.0f, 1.0f };
				pAiMaterial->Get(AI_MATKEY_COLOR_TRANSPARENT, transparent);
				material.transparent.r = transparent.r;
				material.transparent.g = transparent.g;
				material.transparent.b = transparent.b;
			}

			// Reflective
			{
				aiColor3D reflective{ 0.0f, 0.0f, 0.0f };
				pAiMaterial->Get(AI_MATKEY_COLOR_REFLECTIVE, reflective);
				material.reflective.r = reflective.r;
				material.reflective.g = reflective.g;
				material.reflective.b = reflective.b;
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
				material.rendererType = __parseAIType(shadingModel);
			}

			// Blend mode
			{
				aiBlendMode blendMode{ aiBlendMode::aiBlendMode_Default };
				pAiMaterial->Get(AI_MATKEY_BLEND_FUNC, blendMode);
				material.blendOp = __parseAIType(blendMode);
			}

			// Opacity
			{
				float opacity{ 1.0f };
				pAiMaterial->Get(AI_MATKEY_OPACITY, opacity);
				material.ambient.a		= opacity;
				material.diffuse.a		= opacity;
				material.specular.a		= opacity;
				material.emissive.a		= opacity;
				material.transparent.a	= opacity;
				material.reflective.a	= opacity;
			}

			// Shininess
			{
				float shininess{ 0.0f };
				pAiMaterial->Get(AI_MATKEY_SHININESS, shininess);
				material.shininess = shininess;
				if ((material.rendererType == RendererType::GOURAUD) && (shininess > 0.0f))
					material.rendererType = RendererType::PHONG;
			}

			// Shininess strength
			{
				float shininessStrength{ 1.0f };
				pAiMaterial->Get(AI_MATKEY_SHININESS_STRENGTH, shininessStrength);
				material.specular.r *= shininessStrength;
				material.specular.g *= shininessStrength;
				material.specular.b *= shininessStrength;
			}

			// Index of refraction
			{
				float indexOfRefraction{ 1.0f };
				pAiMaterial->Get(AI_MATKEY_REFRACTI, indexOfRefraction);
				material.indexOfRefraction = indexOfRefraction;
			}

			// Texture infos
			for (
				int texTypeIter{ aiTextureType::aiTextureType_NONE };
				texTypeIter < AI_TEXTURE_TYPE_MAX; ++texTypeIter)
			{
				auto const aiTexType	{ static_cast<aiTextureType>(texTypeIter) };
				auto const texInfoCount	{ pAiMaterial->GetTextureCount(aiTexType) };
				if (!texInfoCount)
					continue;

				auto const texType	{ __parseAIType(aiTexType) };
				auto &texInfos		{ material.textureInfoMap[texType] };

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

						uint32_t const texIndex{ texEmplacement.first->second };
						texInfo.index = texIndex;
					}

					// Strength
					{
						float blend{ 1.0f };
						pAiMaterial->Get(AI_MATKEY_TEXBLEND(texTypeIter, texInfoIter), blend);
						texInfo.strength = blend;
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
					}
				}
			}
		}

		// Load textures
		outTextures.resize(texCount);
		std::vector<std::future<void>> bitmapJobs;

		for (auto const &[texName, texIndex] : texNames)
		{
			bitmapJobs.emplace_back(
				std::async(std::launch::async,
					[&outTexture{ outTextures[texIndex] }, texPath{ (assetDir / texName).string() }]
			{
				auto &assetManager	{ Sys::Env::getInstance().getAssetManager() };
				auto const texData	{ assetManager.readBinary(texPath) };
				outTexture = std::make_shared<Infra::Bitmap>(texData.data(), texData.size(), 4ULL);
			}));
		}

		for (auto const &job : bitmapJobs)
			job.wait();
	}

	void ModelLoader::__loadMeshes(
		aiMesh const *const *const pAiMeshes,
		uint32_t const meshCount,
		std::vector<Model::MeshInfo> &outMeshes)
	{
		outMeshes.resize(meshCount);

		for (uint32_t meshIt{ }; meshIt < meshCount; ++meshIt)
		{
			auto const pAiMesh	{ pAiMeshes[meshIt] };
			auto &mesh			{ outMeshes[meshIt] };

			uint32_t const uvChannelCount		{ pAiMesh->GetNumUVChannels() };
			uint32_t const colorChannelCount	{ pAiMesh->GetNumColorChannels() };

			if (uvChannelCount > VertexAttrib::UV_LOCATIONS.size())
			{
				throw std::runtime_error
				{
					std::format(
						"Cannot handle UV channels more than {}.",
						VertexAttrib::UV_LOCATIONS.size())
				};
			}

			mesh.vertexCount = pAiMesh->mNumVertices;

			if (pAiMesh->HasPositions())
			{
				auto &posBuffer{ mesh.vertexBuffers[VertexAttrib::POS_LOCATION] };
				posBuffer.add(
					pAiMesh->mVertices,
					pAiMesh->mNumVertices * VERTEX_ATTRIB_INFOS[VertexAttrib::POS_LOCATION].memSize);
			}

			if (pAiMesh->HasNormals())
			{
				auto &normalBuffer{ mesh.vertexBuffers[VertexAttrib::NORMAL_LOCATION] };
				normalBuffer.add(
					pAiMesh->mNormals,
					pAiMesh->mNumVertices * VERTEX_ATTRIB_INFOS[VertexAttrib::NORMAL_LOCATION].memSize);
			}

			// TODO: Tangents

			if (colorChannelCount)
			{
				auto &colorBuffer{ mesh.vertexBuffers[VertexAttrib::COLOR_LOCATION] };
				colorBuffer.add(
					pAiMesh->mColors[0],
					pAiMesh->mNumVertices * VERTEX_ATTRIB_INFOS[VertexAttrib::COLOR_LOCATION].memSize);

				auto const pColors{ colorBuffer.getTypedData<glm::vec4>() };

				for (uint32_t colorChannelIt{ 1U }; colorChannelIt < colorChannelCount; ++colorChannelIt)
				{
					auto const pAiColors{ pAiMesh->mColors[colorChannelIt] };

					for (uint32_t colorIt{ }; colorIt < pAiMesh->mNumVertices; ++colorIt)
					{
						auto const &srcColor	{ pAiColors[colorIt] };
						auto &dstColor			{ pColors[colorIt] };

						// Porter-Duff "Source Over"
						float const outAlpha = (srcColor.a + (dstColor.a * (1.0f - srcColor.a)));

						for (uint32_t componentIt{ }; componentIt <= 3U; ++componentIt)
						{
							float outColor{ };
							outColor += (srcColor[componentIt] * srcColor.a);
							outColor += (dstColor[componentIt] * dstColor.a * (1.0f - srcColor.a));
							outColor /= outAlpha;

							dstColor[componentIt] = outColor;
						}

						dstColor.a = outAlpha;
					}
				}
			}

			for (uint32_t uvChannelIt{ }; uvChannelIt < uvChannelCount; ++uvChannelIt)
			{
				uint32_t const componentCount{ pAiMesh->mNumUVComponents[uvChannelIt] };
				if (componentCount != 2U)
					throw std::runtime_error{ "Unsupported file format" };

				auto const pAiTexCoords	{ pAiMesh->mTextureCoords[uvChannelIt] };
				auto &uvBuffer			{ mesh.vertexBuffers[VertexAttrib::UV_LOCATIONS[uvChannelIt]] };

				for (uint32_t texCoordIt{ }; texCoordIt < pAiMesh->mNumVertices; ++texCoordIt)
				{
					uvBuffer.add(
						pAiTexCoords + texCoordIt,
						VERTEX_ATTRIB_INFOS[VertexAttrib::UV_LOCATIONS[uvChannelIt]].memSize);
				}
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

	void ModelLoader::__loadDrawInfosAndNodes(
		aiNode const *const pAiRootNode,
		aiMesh const *const *const pAiMeshes,
		std::vector<Model::MeshInfo> const &outMeshes,
		std::vector<Model::DrawInfo> &outDrawInfos,
		std::vector<Model::NodeInfo> &outNodes)
	{
		struct AiNodeSequenceInfo
		{
		public:
			aiNode const *pAiNode{ };
			std::vector<uint32_t> childIndices;
		};

		std::vector<AiNodeSequenceInfo> aiNodeSequence;

		// Serialize
		{
			struct AiNodeStackInfo
			{
			public:
				aiNode const *pAiNode{ };
				std::optional<uint32_t> parent;
			};

			std::stack<AiNodeStackInfo> aiNodeStack;
			aiNodeStack.emplace(pAiRootNode, std::nullopt);

			while (!(aiNodeStack.empty()))
			{
				auto const [pAiNode, parent]{ std::move(aiNodeStack.top()) };
				aiNodeStack.pop();

				uint32_t const nodeIdx{ static_cast<uint32_t>(aiNodeSequence.size()) };
				aiNodeSequence.emplace_back(pAiNode);

				if (parent.has_value())
				{
					auto &parentInfo{ aiNodeSequence[parent.value()] };
					parentInfo.childIndices.emplace_back(nodeIdx);
				}

				for (uint32_t childIter{ }; childIter < pAiNode->mNumChildren; ++childIter)
					aiNodeStack.emplace(pAiNode->mChildren[childIter], nodeIdx);
			}
		}

		outDrawInfos.clear();

		size_t const nodeCount{ aiNodeSequence.size() };
		outNodes.resize(nodeCount);

		for (size_t nodeIdx{ }; nodeIdx < nodeCount; ++nodeIdx)
		{
			auto &[pAiNode, childIndices]	{ aiNodeSequence[nodeIdx] };
			auto &node						{ outNodes[nodeIdx] };

			node.transform = __parseAIType(pAiNode->mTransformation);

			uint32_t const meshCount{ pAiNode->mNumMeshes };
			for (uint32_t meshIt{ }; meshIt < meshCount; ++meshIt)
			{
				uint32_t const drawInfoIndex{ static_cast<uint32_t>(outDrawInfos.size()) };
				node.drawInfoIndices.emplace_back(drawInfoIndex);

				uint32_t const meshIdx	{ pAiNode->mMeshes[meshIt] };
				auto &drawInfo			{ outDrawInfos.emplace_back() };
				drawInfo.meshIdx		= meshIdx;
				drawInfo.materialIdx	= pAiMeshes[meshIdx]->mMaterialIndex;
				drawInfo.indexCount		= outMeshes[meshIdx].indexCount;
				drawInfo.firstIndex		= 0U;
				drawInfo.vertexOffset	= 0;
			}

			node.childIndices = std::move(childIndices);
		}
	}

	glm::mat4 ModelLoader::__parseAIType(
		aiMatrix4x4 const &value) noexcept
	{
		return glm::transpose(reinterpret_cast<glm::mat4 const &>(value));
	}
}