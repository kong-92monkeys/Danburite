#pragma once

#include "../Frameworks/Camera.h"

class FPSCamera : public Infra::Stateful<FPSCamera>
{
public:
	[[nodiscard]]
	constexpr glm::vec3 const &getPosition() const noexcept;
	void setPosition(
		float x,
		float y,
		float z) noexcept;

	void moveLocalX(
		float amount) noexcept;

	void moveLocalY(
		float amount) noexcept;

	void moveLocalZ(
		float amount) noexcept;

	void pitch(
		float amount) noexcept;

	void yaw(
		float amount) noexcept;

	[[nodiscard]]
	constexpr Frx::Camera::ProjectionType getProjectionType() const noexcept;
	void setProjectionType(
		Frx::Camera::ProjectionType type);

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
	constexpr glm::mat4 const &getViewMatrix() const noexcept;

	[[nodiscard]]
	constexpr glm::mat4 const &getProjectionMatrix() const noexcept;

protected:
	virtual void _onValidate() override;

private:
	float __cameraPitch		{ 0.0f };
	float __cameraYaw		{ 0.0f };

	Frx::Camera __camera;

	void __setOrientation() noexcept;
};

constexpr glm::vec3 const &FPSCamera::getPosition() const noexcept
{
	return __camera.getTransform().getPosition().get();
}

constexpr Frx::Camera::ProjectionType FPSCamera::getProjectionType() const noexcept
{
	return __camera.getProjectionType();
}

constexpr float FPSCamera::getWidth() const noexcept
{
	return __camera.getWidth();
}

constexpr float FPSCamera::getHeight() const noexcept
{
	return __camera.getHeight();
}

constexpr float FPSCamera::getNear() const noexcept
{
	return __camera.getNear();
}

constexpr float FPSCamera::getFar() const noexcept
{
	return __camera.getFar();
}

constexpr glm::mat4 const &FPSCamera::getViewMatrix() const noexcept
{
	return __camera.getViewMatrix();
}

constexpr glm::mat4 const &FPSCamera::getProjectionMatrix() const noexcept
{
	return __camera.getProjectionMatrix();
}