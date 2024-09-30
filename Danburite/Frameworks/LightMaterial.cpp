#include "LightMaterial.h"

namespace Frx
{
	void LightMaterial::setColor(
		glm::vec4 const &color)
	{
		_getTypedData().color = color;
		_invokeUpdateEvent();
	}
}