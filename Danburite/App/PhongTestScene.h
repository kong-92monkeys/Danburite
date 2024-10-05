#pragma once

#include "../Frameworks/Scene.h"
#include "../Frameworks/Display.h"
#include "../Frameworks/PhongRenderer.h"
#include "../Frameworks/TransformMaterial.h"
#include "../Frameworks/PhongMaterial.h"
#include "../Frameworks/LightMaterial.h"
#include "FPSCamera.h"
#include "RandomExt.h"

class PhongTestScene : public Frx::Scene
{
public:
	static constexpr size_t MAX_LIGHT_COUNT{ 31U };

	PhongTestScene() = default;
	virtual ~PhongTestScene() noexcept override;

	[[nodiscard]]
	constexpr Frx::Display *getDisplay() const noexcept;

	void setDisplay(
		Frx::Display *pDisplay);

	void syncDisplay();

	void addLight();
	void removeLight();

	constexpr void startCameraMoveAcceleration() noexcept;
	constexpr void endCameraMoveAcceleration() noexcept;

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
	static constexpr uint32_t __CONTAINER_COUNT{ 400U };

	struct __GlobalData
	{
	public:
		glm::mat4 viewMatrix{ 1.0f };
		glm::mat4 projMatrix{ 1.0f };

		alignas(16) glm::vec3 cameraPos{ 0.0f, 0.0f, 0.0f };

		uint32_t lightCount{ };
		std::array<uint32_t, MAX_LIGHT_COUNT> lightIndices{ };
	};

	struct __UpdateParam
	{
	public:
		bool cameraUpdated{ };

		glm::mat4 viewMatrix		{ 1.0f };
		glm::mat4 projMatrix		{ 1.0f };
		glm::vec3 cameraPos			{ 0.0f };

		std::vector<glm::mat4> containerTransforms;
	};

	struct __ObjectTransformInfo
	{
	public:
		glm::vec3 up{ };
		float rotationSpeed{ };
		Frx::Transform transform;
	};

	std::unique_ptr<Frx::PhongRenderer> __rcmd_pRenderer;

	std::unique_ptr<Render::Mesh> __rcmd_pPlaneMesh;
	std::unique_ptr<Render::DrawParam> __rcmd_pPlaneDrawParam;
	std::unique_ptr<Render::Texture> __rcmd_pPlaneTexture;
	std::unique_ptr<Frx::TransformMaterial> __rcmd_pPlaneTransformMaterial;
	std::unique_ptr<Frx::PhongMaterial> __rcmd_pPlanePhongMaterial;
	std::unique_ptr<Render::RenderObject> __rcmd_pPlaneObject;

	std::unique_ptr<Render::Mesh> __rcmd_pContainerMesh;
	std::unique_ptr<Render::DrawParam> __rcmd_pContainerDrawParam;
	std::unique_ptr<Render::Texture> __rcmd_pContainerTexture;
	std::array<std::unique_ptr<Frx::TransformMaterial>, __CONTAINER_COUNT> __rcmd_containerTransformMaterials;
	std::unique_ptr<Frx::PhongMaterial> __rcmd_pContainerPhongMaterial;
	std::unique_ptr<Render::RenderObject> __rcmd_pContainerObject;

	std::unique_ptr<Render::Layer> __rcmd_pLayer;

	Frx::Display *__pDisplay{ };

	float __cameraMoveSpeed		{ 5.0f };
	float __cameraRotationSpeed	{ 0.5f };
	float __cameraFovy			{ glm::pi<float>() / 6.0f };

	bool __cameraMoveAccelerated{ };
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

	std::array<__ObjectTransformInfo, __CONTAINER_COUNT> __containerTransforms;
	FPSCamera __camera;

	std::list<std::unique_ptr<Frx::LightMaterial>> __rcmd_lightMaterials;
	bool __rcmd_lightUpdated{ };

	RandomExt __randomExt;

	__GlobalData __rcmd_globalData;

	void __updateCamera(
		float delta);

	void __syncCameraExtent();
	void __onDisplaySync();

	void __rcmd_createPlaneObject();
	void __rcmd_createContainerObject();
};

constexpr Frx::Display *PhongTestScene::getDisplay() const noexcept
{
	return __pDisplay;
}

constexpr void PhongTestScene::startCameraMoveAcceleration() noexcept
{
	__cameraMoveAccelerated = true;
}

constexpr void PhongTestScene::endCameraMoveAcceleration() noexcept
{
	__cameraMoveAccelerated = false;
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