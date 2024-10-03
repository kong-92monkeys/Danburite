#include "pch.h"
#include "RandomExt.h"

glm::vec3 RandomExt::nextVec3(
	float const min,
	float const max) noexcept
{
	std::uniform_real_distribution<float> dist{ min, max };

	return
	{
		dist(__eng),
		dist(__eng),
		dist(__eng)
	};
}