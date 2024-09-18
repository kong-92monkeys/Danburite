#pragma once

#include <cstdint>

namespace Render::Constants
{
	// NOTE: Need to sync with Constants.glsl

	constexpr size_t DEFAULT_MEMORY_BLOCK_SIZE						{ 16ULL << 20ULL };
	constexpr size_t MAX_IN_FLIGHT_FRAME_COUNT						{ 3ULL };

	// Descriptor set locations
	constexpr uint32_t GLOBAL_DESC_SET_LOCATION						{ 0U };
	constexpr uint32_t SUB_LAYER_DESC_SET_LOCATION					{ 1U };

	// Descriptor locations
	constexpr uint32_t SUB_LAYER_DESC_INSTANCE_INFO_LOCATION		{ 0U };
}