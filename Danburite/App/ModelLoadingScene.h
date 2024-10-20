#pragma once

#include "../Frameworks/Scene.h"
#include "../Frameworks/Display.h"
#include "../Frameworks/PhongRenderer.h"
#include "../Frameworks/TransformMaterial.h"
#include "../Frameworks/PhongMaterial.h"
#include "../Frameworks/LightMaterial.h"
#include "../Frameworks/ModelLoader.h"
#include "FPSCamera.h"
#include "RandomExt.h"

class ModelLoadingScene : public Frx::Scene
{
public:
	ModelLoadingScene() = default;
	virtual ~ModelLoadingScene() noexcept override;

	[[nodiscard]]
	constexpr Frx::Display *getDisplay() const noexcept;

	void setDisplay(
		Frx::Display *pDisplay);

	void syncDisplay();

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

	virtual void _onModelLoaded(
		uint32_t requestIdx,
		Frx::Model::CreateInfo &&result) override;

	virtual void _rcmd_onInit(
		std::any const &initParam) override;

	virtual void _rcmd_onUpdate(
		std::any const &updateParam) override;

private:
	static constexpr size_t __MAX_LIGHT_COUNT{ 3U };

	struct __GlobalData
	{
	public:
		glm::mat4 viewMatrix{ 1.0f };
		glm::mat4 projMatrix{ 1.0f };

		alignas(16) glm::vec3 cameraPos{ 0.0f, 0.0f, 0.0f };

		uint32_t lightCount{ };
		std::array<uint32_t, __MAX_LIGHT_COUNT> lightIndices{ };
	};

	struct __UpdateParam
	{
	public:
		bool cameraUpdated{ };

		glm::mat4 viewMatrix	{ 1.0f };
		glm::mat4 projMatrix	{ 1.0f };
		glm::vec3 cameraPos		{ 0.0f };
	};

	uint32_t __backpackReqId{ };
	std::unique_ptr<Frx::Model> __pBackpack;

	uint32_t __nanosuitReqId{ };
	std::unique_ptr<Frx::Model> __pNanosuit;

	std::unique_ptr<Render::Mesh> __rcmd_pPlaneMesh;
	std::unique_ptr<Render::DrawParam> __rcmd_pPlaneDrawParam;
	std::unique_ptr<Render::Texture> __rcmd_pPlaneTexture;
	std::unique_ptr<Frx::TransformMaterial> __rcmd_pPlaneTransformMaterial;
	std::unique_ptr<Frx::PhongMaterial> __rcmd_pPlanePhongMaterial;
	std::unique_ptr<Render::RenderObject> __rcmd_pPlaneObject;

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

	std::list<std::unique_ptr<Frx::LightMaterial>> __rcmd_lightMaterials;

	FPSCamera __camera;
	RandomExt __randomExt;

	__GlobalData __rcmd_globalData;

	void __updateCamera(
		float delta);

	void __syncCameraExtent();
	void __onDisplaySync();

	void __rcmd_createPlaneObject();
	void __rcmd_addLight();
};

constexpr Frx::Display *ModelLoadingScene::getDisplay() const noexcept
{
	return __pDisplay;
}

constexpr void ModelLoadingScene::startCameraMoveAcceleration() noexcept
{
	__cameraMoveAccelerated = true;
}

constexpr void ModelLoadingScene::endCameraMoveAcceleration() noexcept
{
	__cameraMoveAccelerated = false;
}

constexpr void ModelLoadingScene::startCameraMoveRight() noexcept
{
	__cameraMoveRight = true;
}

constexpr void ModelLoadingScene::endCameraMoveRight() noexcept
{
	__cameraMoveRight = false;
}

constexpr void ModelLoadingScene::startCameraMoveLeft() noexcept
{
	__cameraMoveLeft = true;
}

constexpr void ModelLoadingScene::endCameraMoveLeft() noexcept
{
	__cameraMoveLeft = false;
}

constexpr void ModelLoadingScene::startCameraMoveUp() noexcept
{
	__cameraMoveUp = true;
}

constexpr void ModelLoadingScene::endCameraMoveUp() noexcept
{
	__cameraMoveUp = false;
}

constexpr void ModelLoadingScene::startCameraMoveDown() noexcept
{
	__cameraMoveDown = true;
}

constexpr void ModelLoadingScene::endCameraMoveDown() noexcept
{
	__cameraMoveDown = false;
}

constexpr void ModelLoadingScene::startCameraMoveForward() noexcept
{
	__cameraMoveForward = true;
}

constexpr void ModelLoadingScene::endCameraMoveForward() noexcept
{
	__cameraMoveForward = false;
}

constexpr void ModelLoadingScene::startCameraMoveBackward() noexcept
{
	__cameraMoveBackward = true;
}

constexpr void ModelLoadingScene::endCameraMoveBackward() noexcept
{
	__cameraMoveBackward = false;
}

constexpr void ModelLoadingScene::startCameraRotateRight() noexcept
{
	__cameraRotateRight = true;
}

constexpr void ModelLoadingScene::endCameraRotateRight() noexcept
{
	__cameraRotateRight = false;
}

constexpr void ModelLoadingScene::startCameraRotateLeft() noexcept
{
	__cameraRotateLeft = true;
}

constexpr void ModelLoadingScene::endCameraRotateLeft() noexcept
{
	__cameraRotateLeft = false;
}

constexpr void ModelLoadingScene::startCameraRotateUp() noexcept
{
	__cameraRotateUp = true;
}

constexpr void ModelLoadingScene::endCameraRotateUp() noexcept
{
	__cameraRotateUp = false;
}

constexpr void ModelLoadingScene::startCameraRotateDown() noexcept
{
	__cameraRotateDown = true;
}

constexpr void ModelLoadingScene::endCameraRotateDown() noexcept
{
	__cameraRotateDown = false;
}