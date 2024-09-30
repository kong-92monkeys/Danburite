#include "LightMaterial.h"

namespace Frx
{
	void LightMaterial::setType(
		LightType const type)
	{
		_getTypedData().type = type;
		_invokeUpdateEvent();
	}

	void LightMaterial::setAmbientFactor(
		float const factor)
	{
		_getTypedData().ambientFactor = factor;
		_invokeUpdateEvent();
	}

	void LightMaterial::setColor(
		glm::vec3 const &color)
	{
		_getTypedData().color = color;
		_invokeUpdateEvent();
	}

	void LightMaterial::setPosition(
		glm::vec3 const &position)
	{
		_getTypedData().position = position;
		_invokeUpdateEvent();
	}

	void LightMaterial::setDirection(
		glm::vec3 const &direction)
	{
		_getTypedData().direction = direction;
		_invokeUpdateEvent();
	}
}