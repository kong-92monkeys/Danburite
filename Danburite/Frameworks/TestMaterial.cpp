#include "TestMaterial.h"

namespace Frx
{
	void TestMaterial::setColor(
		glm::vec4 const &color)
	{
		_getTypedData().color = color;
		_invokeUpdateEvent();
	}
}