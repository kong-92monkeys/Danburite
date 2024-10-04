#pragma once

#include "../Render/Engine.h"

namespace Frx::TextureUtil
{
	[[nodiscard]]
	Render::Texture *loadTexture(
		Render::Engine &engine,
		std::string_view const &assetPath,
		bool useMipmap,
		VkPipelineStageFlags2 dstStageMask,
		VkAccessFlags2 dstAccessMask);
}
