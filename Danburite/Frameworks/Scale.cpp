#include "Scale.h"

namespace Frx
{
	void Scale::set(
		float const x,
		float const y,
		float const z)
	{
		__scale.x = x;
		__scale.y = y;
		__scale.z = z;
		_invalidate();
	}

	void Scale::set(
		glm::vec3 const &value)
	{
		set(value.x, value.y, value.z);
	}

	void Scale::set(
		float const value)
	{
		set(value, value, value);
	}

	void Scale::setX(
		float const x)
	{
		__scale.x = x;
		_invalidate();
	}

	void Scale::setY(
		float const y)
	{
		__scale.y = y;
		_invalidate();
	}

	void Scale::setZ(
		float const z)
	{
		__scale.z = z;
		_invalidate();
	}

	void Scale::_onValidate()
	{
		__matrix[0][0] = __scale.x;
		__matrix[1][1] = __scale.y;
		__matrix[2][2] = __scale.z;
	}
}