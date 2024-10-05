#pragma once

#include <assimp/Importer.hpp>
#include <assimp/mesh.h>
#include "../Infra/Unique.h"

namespace Frx
{
	class ModelLoader : public Infra::Unique
	{
	public:
		ModelLoader() = default;

		void filterPrimitiveType(
			aiPrimitiveType type,
			bool filtered) noexcept;

		void load(
			std::string_view const &assetPath);

	private:
		Assimp::Importer __importer;

		int __primitiveTypeFlags{ };
	};
}