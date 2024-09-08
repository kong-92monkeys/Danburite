#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <cstddef>
#include <filesystem>
#include "Unique.h"

namespace Infra
{
	class AssetManager : public Unique
	{
	public:
		AssetManager() = default;

		AssetManager(
			std::string_view const &rootPath) noexcept;

		void setRootPath(
			std::string_view const &rootPath) noexcept;

		[[nodiscard]]
		std::string readString(
			std::string_view const &path) const;

		[[nodiscard]]
		std::vector<std::byte> readBinary(
			std::string_view const &path) const;

	private:
		std::filesystem::path __rootPath;
	};
}