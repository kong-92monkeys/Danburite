#pragma once

#include <assimp/Importer.hpp>
#include "../Infra/Unique.h"

namespace Frx
{
	class ModelLoader : public Infra::Unique
	{
	public:
		ModelLoader() noexcept;

		void load(
			std::string_view const &assetPath);

	private:
		Assimp::Importer __importer;
	};
}
