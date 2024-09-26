#include "TransformMaterial.h"

namespace Frx
{
	void TransformMaterial::setTransform(
		glm::mat4 const &transform) noexcept
	{
		_getTypedData().transform = transform;
		_invokeUpdateEvent();
	}
}