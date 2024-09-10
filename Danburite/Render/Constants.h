#pragma once

#include <cstdint>

namespace Render::Constants
{
	// NOTE: Need to sync with Constants.glsl

	constexpr size_t DEFAULT_MEMORY_BLOCK_SIZE							{ 16ULL << 20ULL };
	constexpr size_t MAX_IN_FLIGHT_FRAME_COUNT_LIMIT					{ 10ULL };

	constexpr uint32_t RENDER_TARGET_DESC_SET_LOCATION					{ 0U };
	// TODO: render target descriptors

	constexpr uint32_t SUB_LAYER_DESC_SET_LOCATION						{ 1U };

	constexpr uint32_t SUB_LAYER_INSTANCE_INFO_LOCATION					{ 0U };

	constexpr uint32_t SUB_LAYER_MATERIAL_DESC_LOCATION0				{ 1U };
	constexpr uint32_t SUB_LAYER_MATERIAL_DESC_LOCATION1				{ 2U };
	constexpr uint32_t SUB_LAYER_MATERIAL_DESC_LOCATION2				{ 3U };
	constexpr uint32_t SUB_LAYER_MATERIAL_DESC_LOCATION3				{ 4U };

	constexpr uint32_t SUB_LAYER_MATERIAL_TEXTURE_LUT_DESC_LOCATION0	{ 5U };
	constexpr uint32_t SUB_LAYER_MATERIAL_TEXTURE_LUT_DESC_LOCATION1	{ 6U };
	constexpr uint32_t SUB_LAYER_MATERIAL_TEXTURE_LUT_DESC_LOCATION2	{ 7U };
	constexpr uint32_t SUB_LAYER_MATERIAL_TEXTURE_LUT_DESC_LOCATION3	{ 8U };

	constexpr uint32_t SUB_LAYER_SAMPLER_LOCATION						{ 9U };
	constexpr uint32_t SUB_LAYER_TEXTURES_LOCATION						{ 10U };
}