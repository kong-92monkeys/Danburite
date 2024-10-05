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

	void LightMaterial::setMaxDistance(
		float const distance)
	{
		_getTypedData().maxDistance = distance;
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

	void LightMaterial::setAttenuation(
		float const constant,
		float const linear,
		float const quad)
	{
		auto &attenuation	{ _getTypedData().attenuation };
		attenuation[0]		= constant;
		attenuation[1]		= linear;
		attenuation[2]		= quad;

		_invokeUpdateEvent();
	}

	void LightMaterial::setCutOff(
		float const inner,
		float const outer)
	{
		_getTypedData().cosInnerCutOff = glm::cos(inner);
		_getTypedData().cosOuterCutOff = glm::cos(outer);
		_invokeUpdateEvent();
	}
}