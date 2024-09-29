#pragma once

#include "Transform.h"

namespace Frx
{
	class Camera : public Infra::Stateful<Camera>
	{
	public:
		enum class ProjectionType
		{
			ORTHO,
			PERSPECTIVE
		};

		Camera() noexcept;

		[[nodiscard]]
		constexpr ProjectionType getProjectionType() const noexcept;
		void setProjectionType(
			ProjectionType type);

		[[nodiscard]]
		constexpr float getWidth() const noexcept;
		void setWidth(
			float width);

		[[nodiscard]]
		constexpr float getHeight() const noexcept;
		void setHeight(
			float height);

		[[nodiscard]]
		constexpr float getNear() const noexcept;
		void setNear(
			float zNear);

		[[nodiscard]]
		constexpr float getFar() const noexcept;
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

		void __calcProjMatrix_ortho() noexcept;
		void __calcProjMatrix_perspective() noexcept;

		ProjectionType __projType{ ProjectionType::PERSPECTIVE };
		void(Camera:: *__calcProjMatrix)() noexcept { &Camera::__calcProjMatrix_perspective };
	};

	constexpr Camera::ProjectionType Camera::getProjectionType() const noexcept
	{
		return __projType;
	}

	constexpr float Camera::getWidth() const noexcept
	{
		return __width;
	}

	constexpr float Camera::getHeight() const noexcept
	{
		return __height;
	}

	constexpr float Camera::getNear() const noexcept
	{
		return __zNear;
	}

	constexpr float Camera::getFar() const noexcept
	{
		return __zFar;
	}

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