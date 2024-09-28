#pragma once

#include <cstdint>

namespace Frx
{
	namespace VertexAttrib
	{
		constexpr uint32_t POS_LOCATION			{ 0U };
		constexpr uint32_t UV_LOCATION			{ 1U };
		constexpr uint32_t NORMAL_LOCATION		{ 2U };
		constexpr uint32_t COLOR_LOCATION		{ 3U };
		constexpr uint32_t TANGENT_LOCATION		{ 4U };
	}

	enum class VertexAttribFlagBits : uint32_t
	{
		NONE		= 0U,

		POS			= (1U << VertexAttrib::POS_LOCATION),
		UV			= (1U << VertexAttrib::UV_LOCATION),
		NORMAL		= (1U << VertexAttrib::NORMAL_LOCATION),
		COLOR		= (1U << VertexAttrib::COLOR_LOCATION),
		TANGENT		= (1U << VertexAttrib::TANGENT_LOCATION)
	};

	enum class VertexAttribFlags : uint32_t
	{
		NONE			= 0U,

		POS				= VertexAttribFlagBits::POS,
		UV				= VertexAttribFlagBits::UV,
		NORMAL			= VertexAttribFlagBits::NORMAL,
		COLOR			= VertexAttribFlagBits::COLOR,
		TANGENT			= VertexAttribFlagBits::TANGENT,

		POS_UV			= (POS | UV),
		POS_NORMAL		= (POS | NORMAL),
		POS_COLOR		= (POS | COLOR)
	};
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