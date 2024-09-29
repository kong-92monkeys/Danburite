#include "Camera.h"

namespace Frx
{
	Camera::Camera() noexcept
	{
		__pTransformInvalidateListener =
			Infra::EventListener<Transform *>::bind(
				&Camera::__onTransformInvalidated, this);

		__transform.getInvalidateEvent() += __pTransformInvalidateListener;
		__validateProjMatrix();
	}

	void Camera::_onValidate()
	{
		if (__viewMatrixInvalidated)
			__validateViewMatrix();

		if (__projMatrixInvalidated)
			__validateProjMatrix();
		
		__viewMatrixInvalidated = false;
		__projMatrixInvalidated = false;
	}

	void Camera::__validateViewMatrix() noexcept
	{
		__transform.validate();

		auto const &position		{ __transform.getPosition().get() };
		auto const &rotation		{ __transform.getOrientation().getMatrix() };
		auto const &scale			{ __transform.getScale().get() };

		glm::mat4 tInv{ 1.0f };
		glm::mat4 rInv{ 1.0f };
		glm::mat4 sInv{ 1.0f };

		tInv[3][0] = -position[0];
		tInv[3][1] = -position[1];
		tInv[3][2] = -position[2];

		rInv = glm::transpose(rotation);

		sInv[0][0] = (1.0f / scale[0]);
		sInv[1][1] = (1.0f / scale[1]);
		sInv[2][2] = (1.0f / scale[2]);

		__viewMatrix = (sInv * rInv * tInv);
	}

	void Camera::__validateProjMatrix() noexcept
	{
		__projMatrix[1][1] = -1.0f;
		__projMatrix[2][2] = -1.0f;
	}

	void Camera::__onTransformInvalidated() noexcept
	{
		__viewMatrixInvalidated = true;
		_invalidate();
	}
}