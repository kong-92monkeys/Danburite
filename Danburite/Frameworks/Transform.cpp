#include "Transform.h"

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