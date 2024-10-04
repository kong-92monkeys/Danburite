#include "pch.h"
#include "RandomExt.h"

bool RandomExt::nextBool() noexcept
{
	std::uniform_int_distribution dist{ 0, 1 };
	return dist(__eng);
}

float RandomExt::nextFloat(
	float const min,
	float const max) noexcept
{
	std::uniform_real_distribution<float> dist{ min, max };
	return dist(__eng);
}

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

glm::vec3 RandomExt::nextVec3(
	float const minX, float const maxX,
	float const minY, float const maxY,
	float const minZ, float const maxZ) noexcept
{
	std::uniform_real_distribution<float> distX{ minX, maxX };
	std::uniform_real_distribution<float> distY{ minY, maxY };
	std::uniform_real_distribution<float> distZ{ minZ, maxZ };

	return
	{
		distX(__eng),
		distY(__eng),
		distZ(__eng)
	};
}