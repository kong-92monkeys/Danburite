#include "Position.h"

namespace Frx
{
	void Position::set(
		float const x,
		float const y,
		float const z)
	{
		__position.x = x;
		__position.y = y;
		__position.z = z;
		_invalidate();
	}

	void Position::set(
		glm::vec3 const &value)
	{
		set(value.x, value.y, value.z);
	}

	void Position::setX(
		float const x)
	{
		__position.x = x;
		_invalidate();
	}

	void Position::setY(
		float const y)
	{
		__position.y = y;
		_invalidate();
	}

	void Position::setZ(
		float const z)
	{
		__position.z = z;
		_invalidate();
	}

	void Position::add(
		float const x,
		float const y,
		float const z)
	{
		__position.x += x;
		__position.y += y;
		__position.z += z;
		_invalidate();
	}

	void Position::add(
		glm::vec3 const &value)
	{
		add(value.x, value.y, value.z);
	}

	void Position::addX(
		float const x)
	{
		__position.x += x;
		_invalidate();
	}

	void Position::addY(
		float const y)
	{
		__position.y += y;
		_invalidate();
	}

	void Position::addZ(
		float const z)
	{
		__position.z += z;
		_invalidate();
	}

	void Position::_onValidate()
	{
		__matrix[3][0] = __position.x;
		__matrix[3][1] = __position.y;
		__matrix[3][2] = __position.z;
	}
}