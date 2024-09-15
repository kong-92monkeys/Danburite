#pragma once

#include <cstdint>

namespace Render::Constants
{
	// NOTE: Need to sync with Constants.glsl

	constexpr size_t DEFAULT_MEMORY_BLOCK_SIZE			{ 16ULL << 20ULL };
	constexpr size_t MAX_IN_FLIGHT_FRAME_COUNT			{ 3ULL };

	constexpr uint32_t MATERIAL_DESC_SET_LOCATION		{ 0U };
}