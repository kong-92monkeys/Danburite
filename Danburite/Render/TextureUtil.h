#pragma once

#include "../Infra/Bitmap.h"
#include "../System/Env.h"
#include "Engine.h"

namespace Render::TextureUtil
{
	[[nodiscard]]
	Texture *loadTexture(
		Engine &engine,
		std::string_view const &assetPath,
		VkPipelineStageFlags2 beforeStageMask,
		VkAccessFlags2 beforeAccessMask,
		VkPipelineStageFlags2 afterStageMask,
		VkAccessFlags2 afterAccessMask);
}
