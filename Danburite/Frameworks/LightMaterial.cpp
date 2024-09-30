#include "LightMaterial.h"

namespace Frx
{
	void LightMaterial::setAmbientFactor(
		float const factor)
	{
		_getTypedData().ambientFactor = factor;
		_invokeUpdateEvent();
	}

	void LightMaterial::setColor(
		glm::vec4 const &color)
	{
		_getTypedData().color = color;
		_invokeUpdateEvent();
	}
}