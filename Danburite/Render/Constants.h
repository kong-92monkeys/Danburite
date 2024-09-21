#pragma once

#include <cstdint>

namespace Render::Constants
{
	// NOTE: Need to sync with Constants.glsl

	constexpr size_t DEFAULT_MEMORY_BLOCK_SIZE						{ 16ULL << 20ULL };
	constexpr size_t MAX_IN_FLIGHT_FRAME_COUNT						{ 3ULL };
	constexpr size_t DEFERRED_DELETER_QUEUE_SIZE					{ MAX_IN_FLIGHT_FRAME_COUNT + 1ULL };

	// Descriptor set locations
	constexpr uint32_t MATERIALS_DESC_SET_LOCATION					{ 0U };
	constexpr uint32_t SAMPLED_IMAGES_DESC_SET_LOCATION				{ 1U };
	constexpr uint32_t SUB_LAYER_DESC_SET_LOCATION					{ 2U };
	constexpr uint32_t RENDERER_DESC_SET_LOCATION					{ 3U };

	// Descriptor locations
	constexpr uint32_t SUB_LAYER_DESC_INSTANCE_INFO_LOCATION		{ 0U };
}