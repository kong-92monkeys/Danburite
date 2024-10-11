#pragma once

#include "../Vulkan/Vulkan.h"
#include "../Infra/GLM.h"
#include <array>

namespace Frx
{
	// NOTE: Need to sync with Constants.glsl

	namespace VertexAttrib
	{
		constexpr uint32_t POS_LOCATION			{ 0U };
		constexpr uint32_t NORMAL_LOCATION		{ 1U };
		constexpr uint32_t TANGENT_LOCATION		{ 2U };
		constexpr uint32_t COLOR_LOCATION		{ 3U };

		constexpr std::array UV_LOCATIONS		{ 4U, 5U, 6U, 7U };
	}

	enum class VertexAttribFlagBits : uint32_t
	{
		NONE		= 0U,

		POS			= (1U << VertexAttrib::POS_LOCATION),
		NORMAL		= (1U << VertexAttrib::NORMAL_LOCATION),
		TANGENT		= (1U << VertexAttrib::TANGENT_LOCATION),
		COLOR		= (1U << VertexAttrib::COLOR_LOCATION),

		UV0			= (1U << VertexAttrib::UV_LOCATIONS[0]),
		UV1			= (1U << VertexAttrib::UV_LOCATIONS[1]),
		UV2			= (1U << VertexAttrib::UV_LOCATIONS[2]),
		UV3			= (1U << VertexAttrib::UV_LOCATIONS[3])
	};

	enum class VertexAttribFlags : uint32_t {};

	struct VertexAttribInfo
	{
	public:
		uint32_t memSize{ };
		VkFormat format{ };
		VertexAttribFlagBits flagBit{ };
	};

	constexpr VertexAttribInfo VERTEX_ATTRIB_INFOS[]
	{
		// Pos
		{ sizeof(glm::vec3), VkFormat::VK_FORMAT_R32G32B32_SFLOAT, VertexAttribFlagBits::POS },

		// Normal
		{ sizeof(glm::vec3), VkFormat::VK_FORMAT_R32G32B32_SFLOAT, VertexAttribFlagBits::NORMAL },

		// Tangent
		{ sizeof(glm::vec3), VkFormat::VK_FORMAT_R32G32B32_SFLOAT, VertexAttribFlagBits::TANGENT },

		// Color
		{ sizeof(glm::vec4), VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT, VertexAttribFlagBits::COLOR },

		// UVs
		{ sizeof(glm::vec2), VkFormat::VK_FORMAT_R32G32_SFLOAT, VertexAttribFlagBits::UV0 },
		{ sizeof(glm::vec2), VkFormat::VK_FORMAT_R32G32_SFLOAT, VertexAttribFlagBits::UV1 },
		{ sizeof(glm::vec2), VkFormat::VK_FORMAT_R32G32_SFLOAT, VertexAttribFlagBits::UV2 },
		{ sizeof(glm::vec2), VkFormat::VK_FORMAT_R32G32_SFLOAT, VertexAttribFlagBits::UV3 }
	};

	constexpr bool hasFlagBit(
		VertexAttribFlags const flags,
		VertexAttribFlagBits const bit) noexcept
	{
		return (static_cast<uint32_t>(flags) & (static_cast<uint32_t>(bit)));
	}

	constexpr bool hasFlagBits(
		VertexAttribFlags const flags,
		VertexAttribFlags const bits) noexcept
	{
		return (
			(static_cast<uint32_t>(flags) & (static_cast<uint32_t>(bits))) ==
			(static_cast<uint32_t>(bits))
		);
	}
}
	
constexpr Frx::VertexAttribFlags operator|(
	Frx::VertexAttribFlagBits const lhs,
	Frx::VertexAttribFlagBits const rhs) noexcept
{
	return static_cast<Frx::VertexAttribFlags>(
		static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr Frx::VertexAttribFlags operator|(
	Frx::VertexAttribFlags const lhs,
	Frx::VertexAttribFlagBits const rhs) noexcept
{
	return static_cast<Frx::VertexAttribFlags>(
		static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr Frx::VertexAttribFlags operator|(
	Frx::VertexAttribFlagBits const lhs,
	Frx::VertexAttribFlags const rhs) noexcept
{
	return (rhs | lhs);
}

constexpr Frx::VertexAttribFlags operator|(
	Frx::VertexAttribFlags const lhs,
	Frx::VertexAttribFlags const rhs) noexcept
{
	return static_cast<Frx::VertexAttribFlags>(
		static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr Frx::VertexAttribFlags operator&(
	Frx::VertexAttribFlags const lhs,
	Frx::VertexAttribFlags const rhs) noexcept
{
	return static_cast<Frx::VertexAttribFlags>(
		static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

constexpr Frx::VertexAttribFlagBits operator&(
	Frx::VertexAttribFlags const lhs,
	Frx::VertexAttribFlagBits const rhs) noexcept
{
	return static_cast<Frx::VertexAttribFlagBits>(
		static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

constexpr Frx::VertexAttribFlagBits operator&(
	Frx::VertexAttribFlagBits const lhs,
	Frx::VertexAttribFlags const rhs) noexcept
{
	return (rhs & lhs);
}

constexpr uint32_t operator&(
	Frx::VertexAttribFlagBits const lhs,
	uint32_t const rhs) noexcept
{
	return (static_cast<uint32_t>(lhs) & rhs);
}

constexpr uint32_t operator&(
	uint32_t const lhs,
	Frx::VertexAttribFlagBits const rhs) noexcept
{
	return (rhs & lhs);
}

constexpr uint32_t operator&(
	Frx::VertexAttribFlags const lhs,
	uint32_t const rhs) noexcept
{
	return (static_cast<uint32_t>(lhs) & rhs);
}

constexpr uint32_t operator&(
	uint32_t const lhs,
	Frx::VertexAttribFlags const rhs) noexcept
{
	return (rhs & lhs);
}

constexpr Frx::VertexAttribFlags &operator|=(
	Frx::VertexAttribFlags &lhs, const Frx::VertexAttribFlagBits rhs) noexcept
{
	lhs = (lhs | rhs);
	return lhs;
}

constexpr Frx::VertexAttribFlags &operator|=(
	Frx::VertexAttribFlags &lhs, const Frx::VertexAttribFlags rhs) noexcept
{
	lhs = (lhs | rhs);
	return lhs;
}