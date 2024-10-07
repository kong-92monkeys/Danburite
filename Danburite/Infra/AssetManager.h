#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <cstddef>
#include <filesystem>
#include <mutex>
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
		bool exists(
			std::string_view const &path) noexcept;

		[[nodiscard]]
		std::string readString(
			std::string_view const &path);

		[[nodiscard]]
		std::vector<std::byte> readBinary(
			std::string_view const &path);

	private:
		std::mutex __mutex;
		std::filesystem::path __rootPath;
	};
}