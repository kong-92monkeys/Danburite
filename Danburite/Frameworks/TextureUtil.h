#pragma once

#include "../Render/Engine.h"

namespace Frx::TextureUtil
{
	[[nodiscard]]
	Render::Texture *loadTexture(
		Render::Engine &engine,
		std::string_view const &assetPath,
		VkPipelineStageFlags2 beforeStageMask,
		VkAccessFlags2 beforeAccessMask,
		VkPipelineStageFlags2 afterStageMask,
		VkAccessFlags2 afterAccessMask);
}
