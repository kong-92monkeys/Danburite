#pragma once

#include <cstdint>

namespace Render::Constants
{
	// NOTE: Need to sync with Constants.glsl

	constexpr size_t DEFAULT_MEMORY_BLOCK_SIZE				{ 16ULL << 20ULL };
	constexpr size_t MAX_IN_FLIGHT_FRAME_COUNT				{ 3ULL };
	constexpr size_t DEFERRED_DELETER_QUEUE_SIZE			{ MAX_IN_FLIGHT_FRAME_COUNT + 1ULL };

	// Descriptor set locations
	constexpr uint32_t GLOBAL_DESC_SET_LOCATION				{ 0U };
	constexpr uint32_t MATERIALS_DESC_SET_LOCATION			{ 1U };
	constexpr uint32_t SAMPLED_IMAGES_DESC_SET_LOCATION		{ 2U };
	constexpr uint32_t SUB_LAYER_DESC_SET_LOCATION			{ 3U };
	constexpr uint32_t RENDERER_DESC_SET_LOCATION			{ 4U };

	// Descriptor locations

	// Global desc set
	constexpr uint32_t GLOBAL_DATA_BUFFER_LOCATION		{ 0U };

	// Sampled images desc set
	constexpr uint32_t SAMPLED_IMAGES_LOCATION			{ 0U };

	// Sub layer desc set
	constexpr uint32_t INSTANCE_INFO_BUFFER_LOCATION	{ 0U };
}