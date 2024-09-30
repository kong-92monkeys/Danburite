#include "TransformMaterial.h"

namespace Frx
{
	void TransformMaterial::setTransform(
		glm::mat4 const &transform)
	{
		_getTypedData().transform		= transform;
		_getTypedData().normalTransform	= glm::transpose(glm::inverse(glm::mat3{ transform }));
		_invokeUpdateEvent();
	}
}