#include "GouraudMaterial.h"

namespace Frx
{
	void GouraudMaterial::setAmbient(
		glm::vec3 const &color)
	{
		_getTypedData().ambient = color;
		_invokeUpdateEvent();
	}

	void GouraudMaterial::setDiffuse(
		glm::vec3 const &color)
	{
		_getTypedData().diffuse = color;
		_invokeUpdateEvent();
	}

	void GouraudMaterial::setEmissive(
		glm::vec3 const &color)
	{
		_getTypedData().emissive = color;
		_invokeUpdateEvent();
	}

	void GouraudMaterial::setBlendOp(
		ColorBlendOp const blendOp)
	{
		_getTypedData().blendOp = blendOp;
		_invokeUpdateEvent();
	}
}