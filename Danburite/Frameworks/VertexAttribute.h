#pragma once

#include <cstdint>

namespace Frx
{
	namespace VertexAttrib
	{
		constexpr uint32_t POS_LOCATION			{ 0U };
		constexpr uint32_t TEXCOORD_LOCATION	{ 1U };
		constexpr uint32_t NORMAL_LOCATION		{ 2U };
		constexpr uint32_t COLOR_LOCATION		{ 3U };
		constexpr uint32_t TANGENT_LOCATION		{ 4U };
	}

	enum class VertexAttribFlagBits : uint32_t
	{
		NONE		= 0U,

		POS			= (1U << VertexAttrib::POS_LOCATION),
		TEXCOORD	= (1U << VertexAttrib::TEXCOORD_LOCATION),
		NORMAL		= (1U << VertexAttrib::NORMAL_LOCATION),
		COLOR		= (1U << VertexAttrib::COLOR_LOCATION),
		TANGENT		= (1U << VertexAttrib::TANGENT_LOCATION)
	};

	enum class VertexAttribFlags : uint32_t
	{
		NONE			= 0U,

		POS				= VertexAttribFlagBits::POS,
		TEXCOORD		= VertexAttribFlagBits::TEXCOORD,
		NORMAL			= VertexAttribFlagBits::NORMAL,
		COLOR			= VertexAttribFlagBits::COLOR,
		TANGENT			= VertexAttribFlagBits::TANGENT,

		POS_TEXCOORD	= (POS | TEXCOORD),
		POS_NORMAL		= (POS | NORMAL),
		POS_COLOR		= (POS | COLOR)
	};
}
	
constexpr Frx::VertexAttribFlags operator|(
	const Frx::VertexAttribFlagBits lhs, const Frx::VertexAttribFlagBits rhs) noexcept
{
	return static_cast<Frx::VertexAttribFlags>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr Frx::VertexAttribFlags operator|(
	const Frx::VertexAttribFlags lhs, const Frx::VertexAttribFlagBits rhs) noexcept
{
	return static_cast<Frx::VertexAttribFlags>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr Frx::VertexAttribFlags operator|(
	const Frx::VertexAttribFlagBits lhs, const Frx::VertexAttribFlags rhs) noexcept
{
	return static_cast<Frx::VertexAttribFlags>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr Frx::VertexAttribFlags operator|(
	const Frx::VertexAttribFlags lhs, const Frx::VertexAttribFlags rhs) noexcept
{
	return static_cast<Frx::VertexAttribFlags>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr Frx::VertexAttribFlags operator&(
	const Frx::VertexAttribFlags lhs, const Frx::VertexAttribFlags rhs) noexcept
{
	return static_cast<Frx::VertexAttribFlags>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

constexpr Frx::VertexAttribFlagBits operator&(
	const Frx::VertexAttribFlags lhs, const Frx::VertexAttribFlagBits rhs) noexcept
{
	return static_cast<Frx::VertexAttribFlagBits>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

constexpr Frx::VertexAttribFlagBits operator&(
	const Frx::VertexAttribFlagBits lhs, const Frx::VertexAttribFlags rhs) noexcept
{
	return static_cast<Frx::VertexAttribFlagBits>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
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