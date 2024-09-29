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

	void Camera::setWidth(
		float const width)
	{
		__width = width;
		__projMatrixInvalidated = true;
		_invalidate();
	}

	void Camera::setHeight(
		float const height)
	{
		__height = height;
		__projMatrixInvalidated = true;
		_invalidate();
	}

	void Camera::setNear(
		float const zNear)
	{
		__zNear = zNear;
		__projMatrixInvalidated = true;
		_invalidate();
	}

	void Camera::setFar(
		float const zFar)
	{
		__zFar = zFar;
		__projMatrixInvalidated = true;
		_invalidate();
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
		__projMatrix[0][0] = (2.0f / __width);
		__projMatrix[1][1] = -(2.0f / __height);
		__projMatrix[2][2] = (1.0f / (__zFar - __zNear));
		__projMatrix[3][2] = (__zFar / (__zFar - __zNear));
	}

	void Camera::__onTransformInvalidated() noexcept
	{
		__viewMatrixInvalidated = true;
		_invalidate();
	}
}