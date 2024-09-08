#pragma once

#include <cstdint>

#include <compare>

namespace Infra
{
	struct Version
	{
	public:
		uint32_t major{ };
		uint32_t minor{ };
		uint32_t patch{ };
		uint32_t variant{ };

		[[nodiscard]]
		constexpr std::strong_ordering operator<=>(
			Version const &other) const noexcept;
	};

	constexpr std::strong_ordering Version::operator<=>(
		Version const &other) const noexcept
	{
		if (major < other.major)
			return std::strong_ordering::less;
		else if (major > other.major)
			return std::strong_ordering::greater;

		if (minor < other.minor)
			return std::strong_ordering::less;
		else if (minor > other.minor)
			return std::strong_ordering::greater;

		if (patch < other.patch)
			return std::strong_ordering::less;
		else if (patch > other.patch)
			return std::strong_ordering::greater;

		if (variant < other.variant)
			return std::strong_ordering::less;
		else if (variant > other.variant)
			return std::strong_ordering::greater;

		return std::strong_ordering::equal;
	}
}