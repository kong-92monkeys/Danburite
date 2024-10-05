#include "ModelLoader.h"
#include "../System/Env.h"
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>

namespace Frx
{
	ModelLoader::ModelLoader() noexcept
	{
		__importer.SetPropertyInteger(
			AI_CONFIG_PP_SBP_REMOVE,
			aiPrimitiveType::aiPrimitiveType_POINT | aiPrimitiveType::aiPrimitiveType_LINE);
	}

	void ModelLoader::load(
		std::string_view const &assetPath)
	{
		auto &assetManager		{ Sys::Env::getInstance().getAssetManager() };
		auto const binary		{ assetManager.readBinary(assetPath) };

		const aiScene *scene = __importer.ReadFileFromMemory(
			binary.data(),
			binary.size(),
			aiPostProcessSteps::aiProcess_Triangulate |
			aiPostProcessSteps::aiProcess_JoinIdenticalVertices |
			aiPostProcessSteps::aiProcess_SortByPType);

		__importer.FreeScene();
	}
}