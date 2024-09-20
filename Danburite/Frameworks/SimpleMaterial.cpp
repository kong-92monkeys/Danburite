#include "SimpleMaterial.h"

namespace Frx
{
	void SimpleMaterial::setColor(
		glm::vec4 const &color) noexcept
	{
		_getTypedData().color = color;
		_invokeUpdateEvent();
	}
}