#pragma once

#include <assimp/Importer.hpp>
#include <assimp/mesh.h>
#include <assimp/scene.h>
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
		static std::vector<Model::Mesh> __loadMesh(
			aiMesh const *const *mMeshes,
			uint32_t mNumMeshes);

		[[nodiscard]]
		static std::vector<Model::Material> __loadMaterial(
			aiMaterial const *const *mMaterials,
			uint32_t mNumMaterials);
	};
}