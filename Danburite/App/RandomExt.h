#include <random>
#include "../Infra/GLM.h"

class RandomExt
{
public:
	[[nodiscard]]
	glm::vec3 nextVec3(
		float min, float max) noexcept;

private:
	std::default_random_engine __eng;
};