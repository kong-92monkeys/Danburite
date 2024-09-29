#pragma once

#include "../Frameworks/Camera.h"

class FPSCamera : public Infra::Stateful<FPSCamera>
{
public:
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

	void setWidth(
		float width);

	void setHeight(
		float height);

	void setNear(
		float zNear);

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

constexpr glm::mat4 const &FPSCamera::getViewMatrix() const noexcept
{
	return __camera.getViewMatrix();
}

constexpr glm::mat4 const &FPSCamera::getProjectionMatrix() const noexcept
{
	return __camera.getProjectionMatrix();
}