#pragma once

#include "Transform.h"

namespace Frx
{
	class Camera : public Infra::Stateful<Camera>
	{
	public:
		Camera() noexcept;

		void setWidth(
			float width);

		void setHeight(
			float height);

		void setNear(
			float zNear);

		void setFar(
			float zFar);

		[[nodiscard]]
		constexpr Transform &getTransform() noexcept;

		[[nodiscard]]
		constexpr Transform const &getTransform() const noexcept;

		[[nodiscard]]
		constexpr glm::mat4 const &getViewMatrix() const noexcept;

		[[nodiscard]]
		constexpr glm::mat4 const &getProjectionMatrix() const noexcept;

	protected:
		virtual void _onValidate() override;

	private:
		Transform __transform;

		float __width	{ 400.0f };
		float __height	{ 300.0f };

		float __zNear	{ 1.0f };
		float __zFar	{ 1000.0f };

		bool __viewMatrixInvalidated{ };
		glm::mat4 __viewMatrix{ 1.0f };

		bool __projMatrixInvalidated{ };
		glm::mat4 __projMatrix{ 1.0f };

		Infra::EventListenerPtr<Transform *> __pTransformInvalidateListener;

		void __validateViewMatrix() noexcept;
		void __validateProjMatrix() noexcept;

		void __onTransformInvalidated() noexcept;
	};

	constexpr Transform &Camera::getTransform() noexcept
	{
		return __transform;
	}

	constexpr Transform const &Camera::getTransform() const noexcept
	{
		return __transform;
	}

	constexpr glm::mat4 const &Camera::getViewMatrix() const noexcept
	{
		return __viewMatrix;
	}

	constexpr glm::mat4 const &Camera::getProjectionMatrix() const noexcept
	{
		return __projMatrix;
	}
}