#pragma once

#include <cstdint>

namespace Frx::Constants
{
	constexpr uint32_t MAX_TEX_CHANNEL_COUNT					{ 4U };

	// Descriptor locations

	// MaterialInfo desc set
	constexpr uint32_t TRANSFORM_MATERIAL_LOCATION				{ 0U };
	constexpr uint32_t PHONG_MATERIAL_LOCATION					{ 1U };
	constexpr uint32_t LIGHT_MATERIAL_LOCATION					{ 2U };

	// Renderer desc set
	constexpr uint32_t PHONG_RENDERER_IMAGE_SAMLER_LOCATION		{ 0U };
}