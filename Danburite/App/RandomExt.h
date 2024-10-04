#include <random>
#include "../Infra/GLM.h"

class RandomExt
{
public:
	[[nodiscard]]
	bool nextBool() noexcept;

	[[nodiscard]]
	float nextFloat(
		float min, float max) noexcept;

	[[nodiscard]]
	glm::vec3 nextVec3(
		float min, float max) noexcept;

	[[nodiscard]]
	glm::vec3 nextVec3(
		float minX, float maxX,
		float minY, float maxY, 
		float minZ, float maxZ) noexcept;

private:
	std::default_random_engine __eng;
};