#include "ModelLoader.h"
#include "../System/Env.h"
#include <assimp/scene.h>
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

	void ModelLoader::load(
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
				aiPostProcessSteps::aiProcess_SortByPType)
		};

		if (!pScene)
			throw std::runtime_error{ __importer.GetErrorString() };

		// NOTE: Need to transpose matrix

		__importer.FreeScene();
	}
}