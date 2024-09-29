#include "Orientation.h"

namespace Frx
{
	void Orientation::set(
		float const w,
		float const x,
		float const y,
		float const z)
	{
		__orientation.w = w;
		__orientation.x = x;
		__orientation.y = y;
		__orientation.z = z;
		_invalidate();
	}

	void Orientation::set(
		glm::quat const &value)
	{
		set(value.w, value.x, value.y, value.z);
	}

	void Orientation::rotate(
		glm::quat const &value)
	{
		__orientation = (value * __orientation);
		__orientation = glm::normalize(__orientation);
		_invalidate();
	}

	void Orientation::rotate(
		float const angle,
		glm::vec3 const &axis)
	{
		rotate(glm::angleAxis(angle, axis));
	}

	void Orientation::_onValidate()
	{
		__matrix = glm::mat4_cast(__orientation);
	}
}