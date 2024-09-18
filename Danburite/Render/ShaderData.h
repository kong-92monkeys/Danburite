#pragma once

#include "../Infra/GLM.h"

namespace Render
{
	struct InstanceInfo
	{
	public:
		glm::ivec4 materialIds{ -1, -1, -1, -1 };

		constexpr void reset() noexcept;
	};

	constexpr void InstanceInfo::reset() noexcept
	{
		materialIds[0] = -1;
		materialIds[1] = -1;
		materialIds[2] = -1;
		materialIds[3] = -1;
	}
}
