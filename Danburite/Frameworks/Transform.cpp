#include "Transform.h"
#include <glm/gtx/matrix_decompose.hpp>

namespace Frx
{
	Transform::Transform() noexcept
	{
		__pPositionInvalidateListener =
			Infra::EventListener<Position *>::bind(
				&Transform::__onPositionInvalidated, this);

		__pOrientationInvalidateListener =
			Infra::EventListener<Orientation *>::bind(
				&Transform::__onOrientationInvalidated, this);

		__pScaleInvalidateListener =
			Infra::EventListener<Scale *>::bind(
				&Transform::__onScaleInvalidated, this);

		__position.getInvalidateEvent() += __pPositionInvalidateListener;
		__orientation.getInvalidateEvent() += __pOrientationInvalidateListener;
		__scale.getInvalidateEvent() += __pScaleInvalidateListener;
	}

	Transform::Transform(
		glm::mat4 const &src)
	{
		glm::vec3 scale{ };
		glm::quat orientation{ };
		glm::vec3 position{ };
		glm::vec3 skew{ };
		glm::vec4 perspective{ };

		if (!(glm::decompose(src, scale, orientation, position, skew, perspective)))
			throw std::runtime_error{ "Cannot decompose a singular matrix." };

		__position.set(position);
		__orientation.set(orientation);
		__scale.set(scale);
		_onValidate();

		__pPositionInvalidateListener =
			Infra::EventListener<Position *>::bind(
				&Transform::__onPositionInvalidated, this);

		__pOrientationInvalidateListener =
			Infra::EventListener<Orientation *>::bind(
				&Transform::__onOrientationInvalidated, this);

		__pScaleInvalidateListener =
			Infra::EventListener<Scale *>::bind(
				&Transform::__onScaleInvalidated, this);

		__position.getInvalidateEvent() += __pPositionInvalidateListener;
		__orientation.getInvalidateEvent() += __pOrientationInvalidateListener;
		__scale.getInvalidateEvent() += __pScaleInvalidateListener;
	}

	void Transform::setMatrix(
		glm::mat4 const &src)
	{
		glm::vec3 scale{ };
		glm::quat orientation{ };
		glm::vec3 position{ };
		glm::vec3 skew{ };
		glm::vec4 perspective{ };

		if (!(glm::decompose(src, scale, orientation, position, skew, perspective)))
			throw std::runtime_error{ "Cannot decompose a singular matrix." };

		__position.set(position);
		__orientation.set(orientation);
		__scale.set(scale);
	}

	void Transform::_onValidate()
	{
		__position.validate();
		__orientation.validate();
		__scale.validate();

		__matrix = (__position.getMatrix() * __orientation.getMatrix() * __scale.getMatrix());
	}

	void Transform::__onPositionInvalidated() noexcept
	{
		_invalidate();
	}

	void Transform::__onOrientationInvalidated() noexcept
	{
		_invalidate();
	}

	void Transform::__onScaleInvalidated() noexcept
	{
		_invalidate();
	}
}