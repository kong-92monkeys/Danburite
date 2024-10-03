#pragma once

#include "../Frameworks/Scene.h"
#include "../Frameworks/Display.h"
#include "../Frameworks/PhongRenderer.h"
#include "../Frameworks/TransformMaterial.h"
#include "../Frameworks/PhongMaterial.h"
#include "../Frameworks/LightMaterial.h"
#include "FPSCamera.h"

class PhongTestScene : public Frx::Scene
{
public:
	PhongTestScene() = default;
	virtual ~PhongTestScene() noexcept override;

	[[nodiscard]]
	constexpr Frx::Display *getDisplay() const noexcept;

	void setDisplay(
		Frx::Display *pDisplay);

	void syncDisplay();

	constexpr void startCameraMoveRight() noexcept;
	constexpr void endCameraMoveRight() noexcept;

	constexpr void startCameraMoveLeft() noexcept;
	constexpr void endCameraMoveLeft() noexcept;

	constexpr void startCameraMoveUp() noexcept;
	constexpr void endCameraMoveUp() noexcept;

	constexpr void startCameraMoveDown() noexcept;
	constexpr void endCameraMoveDown() noexcept;

	constexpr void startCameraMoveForward() noexcept;
	constexpr void endCameraMoveForward() noexcept;

	constexpr void startCameraMoveBackward() noexcept;
	constexpr void endCameraMoveBackward() noexcept;

	constexpr void startCameraRotateRight() noexcept;
	constexpr void endCameraRotateRight() noexcept;

	constexpr void startCameraRotateLeft() noexcept;
	constexpr void endCameraRotateLeft() noexcept;

	constexpr void startCameraRotateUp() noexcept;
	constexpr void endCameraRotateUp() noexcept;

	constexpr void startCameraRotateDown() noexcept;
	constexpr void endCameraRotateDown() noexcept;

protected:
	[[nodiscard]]
	virtual std::any _onInit() override;

	[[nodiscard]]
	virtual std::any _onUpdate(
		Time const &time) override;

	virtual void _rcmd_onInit(
		std::any const &initParam) override;

	virtual void _rcmd_onUpdate(
		std::any const &updateParam) override;

private:
	struct __GlobalData
	{
	public:
		glm::mat4 viewMatrix{ 1.0f };
		glm::mat4 projMatrix{ 1.0f };
		alignas(16) glm::vec3 cameraPosition{ 0.0f, 0.0f, 0.0f };
		int lightIdx{ -1 };
	};

	struct __UpdateParam
	{
	public:
		glm::mat4 objectTransform{ };

		bool globalDataUpdated{ };
		__GlobalData globalData{ };
	};

	std::unique_ptr<Render::Mesh> __rcmd_pMesh;
	std::unique_ptr<Render::DrawParam> __rcmd_pDrawParam;
	std::unique_ptr<Frx::PhongRenderer> __rcmd_pRenderer;
	std::unique_ptr<Render::Texture> __rcmd_pAlbedoTexture;
	std::unique_ptr<Frx::TransformMaterial> __rcmd_pTransformMaterial;
	std::unique_ptr<Frx::PhongMaterial> __rcmd_pPhongMaterial;
	std::unique_ptr<Render::RenderObject> __rcmd_pObject;
	std::unique_ptr<Render::Layer> __rcmd_pLayer;

	std::unique_ptr<Frx::LightMaterial> __rcmd_pLightMaterial;
	int __rcmd_lightIdx{ -1 };

	Frx::Display *__pDisplay{ };

	float __cameraMoveSpeed		{ 1.0f };
	float __cameraRotationSpeed	{ 0.5f };
	float __cameraFovy			{ glm::pi<float>() / 6.0f };

	float __objectRotationSpeed	{ 1.0f };

	bool __cameraMoveLeft{ };
	bool __cameraMoveRight{ };
	bool __cameraMoveUp{ };
	bool __cameraMoveDown{ };
	bool __cameraMoveForward{ };
	bool __cameraMoveBackward{ };

	bool __cameraRotateRight{ };
	bool __cameraRotateLeft{ };
	bool __cameraRotateUp{ };
	bool __cameraRotateDown{ };

	FPSCamera __camera;
	Frx::Transform __objectTransform;

	void __handleCamera(
		float delta);

	void __syncCameraExtent();
	void __onDisplaySync();
};

constexpr Frx::Display *PhongTestScene::getDisplay() const noexcept
{
	return __pDisplay;
}

constexpr void PhongTestScene::startCameraMoveRight() noexcept
{
	__cameraMoveRight = true;
}

constexpr void PhongTestScene::endCameraMoveRight() noexcept
{
	__cameraMoveRight = false;
}

constexpr void PhongTestScene::startCameraMoveLeft() noexcept
{
	__cameraMoveLeft = true;
}

constexpr void PhongTestScene::endCameraMoveLeft() noexcept
{
	__cameraMoveLeft = false;
}

constexpr void PhongTestScene::startCameraMoveUp() noexcept
{
	__cameraMoveUp = true;
}

constexpr void PhongTestScene::endCameraMoveUp() noexcept
{
	__cameraMoveUp = false;
}

constexpr void PhongTestScene::startCameraMoveDown() noexcept
{
	__cameraMoveDown = true;
}

constexpr void PhongTestScene::endCameraMoveDown() noexcept
{
	__cameraMoveDown = false;
}

constexpr void PhongTestScene::startCameraMoveForward() noexcept
{
	__cameraMoveForward = true;
}

constexpr void PhongTestScene::endCameraMoveForward() noexcept
{
	__cameraMoveForward = false;
}

constexpr void PhongTestScene::startCameraMoveBackward() noexcept
{
	__cameraMoveBackward = true;
}

constexpr void PhongTestScene::endCameraMoveBackward() noexcept
{
	__cameraMoveBackward = false;
}

constexpr void PhongTestScene::startCameraRotateRight() noexcept
{
	__cameraRotateRight = true;
}

constexpr void PhongTestScene::endCameraRotateRight() noexcept
{
	__cameraRotateRight = false;
}

constexpr void PhongTestScene::startCameraRotateLeft() noexcept
{
	__cameraRotateLeft = true;
}

constexpr void PhongTestScene::endCameraRotateLeft() noexcept
{
	__cameraRotateLeft = false;
}

constexpr void PhongTestScene::startCameraRotateUp() noexcept
{
	__cameraRotateUp = true;
}

constexpr void PhongTestScene::endCameraRotateUp() noexcept
{
	__cameraRotateUp = false;
}

constexpr void PhongTestScene::startCameraRotateDown() noexcept
{
	__cameraRotateDown = true;
}

constexpr void PhongTestScene::endCameraRotateDown() noexcept
{
	__cameraRotateDown = false;
}