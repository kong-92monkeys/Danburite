#include "pch.h"
#include "PhongTestScene.h"
#include "../Frameworks/PrimitiveBuilder.h"
#include <glm/gtc/matrix_transform.hpp>

PhongTestScene::PhongTestScene() noexcept
{
	__pDisplaySyncListener =
		Infra::EventListener<Frx::Display const *>::bind(
			&PhongTestScene::__onDisplaySync, this);
}

PhongTestScene::~PhongTestScene() noexcept
{
	_stopLoop();
	_rcmd_run([this]
	{
		_rcmd_removeGlobalMaterial(__rcmd_pLightMaterial.get());

		if (__pDisplay)
			__pDisplay->rcmd_getRenderTarget().removeLayer(__rcmd_pLayer.get());

		__rcmd_pLightMaterial = nullptr;
		__rcmd_pLayer = nullptr;
		__rcmd_pObject = nullptr;
		__rcmd_pPhongMaterial = nullptr;
		__rcmd_pTransformMaterial = nullptr;
		__rcmd_pAlbedoTexture = nullptr;
		__rcmd_pRenderer = nullptr;
		__rcmd_pDrawParam = nullptr;
		__rcmd_pMesh = nullptr;
	}).wait();
}

void PhongTestScene::setDisplay(
	Frx::Display *const pDisplay)
{
	if (__pDisplay == pDisplay)
		return;

	auto const pPrevDisplay{ __pDisplay };
	__pDisplay = pDisplay;

	if (pPrevDisplay)
		pPrevDisplay->getSyncEvent() -= __pDisplaySyncListener;

	if (pDisplay)
	{
		pDisplay->getSyncEvent() += __pDisplaySyncListener;

		if (__pDisplay->isPresentable())
			__syncCameraExtent();
	}

	_rcmd_run([this, pPrevDisplay, pDisplay]
	{
		if (pPrevDisplay)
			pPrevDisplay->rcmd_getRenderTarget().removeLayer(__rcmd_pLayer.get());

		if (pDisplay)
			pDisplay->rcmd_getRenderTarget().addLayer(__rcmd_pLayer.get());
	}).wait();
}

std::any PhongTestScene::_scmd_onInit()
{
	__scmd_camera.setPosition(0.0f, 0.0f, 3.0f);
	__scmd_camera.setNear(0.1f);
	__scmd_camera.validate();

	return __GlobalData
	{
		.viewMatrix{ __scmd_camera.getViewMatrix() },
		.projMatrix{ __scmd_camera.getProjectionMatrix() }
	};
}

std::any PhongTestScene::_scmd_onUpdate(
	Time const &time)
{
	float const delta{ static_cast<float>(time.deltaTime.count() * 1.0e-9) };
	__scmd_handleCamera(delta);

	__UpdateParam retVal;

	if (__scmd_camera.isInvalidated())
	{
		__scmd_camera.validate();

		retVal.cameraUpdated = true;
		retVal.globalData.viewMatrix = __scmd_camera.getViewMatrix();
		retVal.globalData.projMatrix = __scmd_camera.getProjectionMatrix();
	}

	return retVal;
}

void PhongTestScene::_rcmd_onInit(
	std::any const &initParam)
{
	auto const meshData
	{
		Frx::PrimitiveBuilder::buildCube(
			Frx::VertexAttribFlags::POS_UV_NORMAL_COLOR, 1.0f)
	};

	__rcmd_pMesh = _rcmd_createMesh();
	__rcmd_pMesh->createVertexBuffer(Frx::VertexAttrib::POS_LOCATION, meshData.posBuffer.getData(), meshData.posBuffer.getSize());
	__rcmd_pMesh->createVertexBuffer(Frx::VertexAttrib::UV_LOCATION, meshData.uvBuffer.getData(), meshData.uvBuffer.getSize());
	__rcmd_pMesh->createVertexBuffer(Frx::VertexAttrib::NORMAL_LOCATION, meshData.normalBuffer.getData(), meshData.normalBuffer.getSize());
	__rcmd_pMesh->createVertexBuffer(Frx::VertexAttrib::COLOR_LOCATION, meshData.colorBuffer.getData(), meshData.colorBuffer.getSize());
	__rcmd_pMesh->createIndexBuffer(meshData.indexType, meshData.indexBuffer.getData(), meshData.indexBuffer.getSize());

	__rcmd_pDrawParam = std::make_unique<Render::DrawParamIndexed>(meshData.indexCount, 0U, 0);
	__rcmd_pRenderer = _rcmd_createRenderer<Frx::PhongRenderer>();

	__rcmd_pAlbedoTexture = _rcmd_createTexture(
		"Images/smile.jpg",
		VK_PIPELINE_STAGE_2_NONE, VK_ACCESS_2_NONE,
		VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
		VK_ACCESS_2_SHADER_SAMPLED_READ_BIT);

	__rcmd_pTransformMaterial = _rcmd_createMaterial<Frx::TransformMaterial>();

	__rcmd_pPhongMaterial = _rcmd_createMaterial<Frx::PhongMaterial>();
	__rcmd_pPhongMaterial->setAlbedoTexture(__rcmd_pAlbedoTexture.get());

	__rcmd_pObject = std::make_unique<Render::RenderObject>();
	__rcmd_pObject->setMesh(__rcmd_pMesh.get());
	__rcmd_pObject->setDrawParam(__rcmd_pDrawParam.get());
	__rcmd_pObject->setRenderer(__rcmd_pRenderer.get());
	__rcmd_pObject->getMaterialPackOf(0U).setMaterial(__rcmd_pTransformMaterial.get());
	__rcmd_pObject->getMaterialPackOf(0U).setMaterial(__rcmd_pPhongMaterial.get());

	__rcmd_pLayer = _rcmd_createLayer();
	__rcmd_pLayer->addRenderObject(__rcmd_pObject.get());

	__rcmd_pLightMaterial = _rcmd_createMaterial<Frx::LightMaterial>();
	__rcmd_pLightMaterial->setColor(glm::vec4{ 1.0f, 0.5f, 0.0f, 1.0f });

	_rcmd_addGlobalMaterial(__rcmd_pLightMaterial.get());

	auto const globalData{ std::any_cast<__GlobalData>(initParam) };
	_rcmd_setGlobalData(globalData);
}

void PhongTestScene::_rcmd_onUpdate(
	std::any const &updateParam)
{
	if (!__pDisplay)
		return;

	auto const param{ std::any_cast<__UpdateParam>(updateParam) };
	if (!(param.cameraUpdated))
		return;

	_rcmd_setGlobalData(param.globalData);

	auto &renderTarget{ __pDisplay->rcmd_getRenderTarget() };
	renderTarget.requestRedraw();
}

void PhongTestScene::__scmd_handleCamera(
	float const delta)
{
	if (__cameraMoveRight)
		__scmd_camera.moveLocalX(delta * __cameraMoveSpeed);

	if (__cameraMoveLeft)
		__scmd_camera.moveLocalX(-delta * __cameraMoveSpeed);

	if (__cameraMoveUp)
		__scmd_camera.moveLocalY(delta * __cameraMoveSpeed);

	if (__cameraMoveDown)
		__scmd_camera.moveLocalY(-delta * __cameraMoveSpeed);

	if (__cameraMoveForward)
		__scmd_camera.moveLocalZ(-delta * __cameraMoveSpeed);

	if (__cameraMoveBackward)
		__scmd_camera.moveLocalZ(delta * __cameraMoveSpeed);

	if (__cameraRotateRight)
		__scmd_camera.yaw(-delta * __cameraRotationSpeed);

	if (__cameraRotateLeft)
		__scmd_camera.yaw(delta * __cameraRotationSpeed);

	if (__cameraRotateUp)
		__scmd_camera.pitch(delta * __cameraRotationSpeed);

	if (__cameraRotateDown)
		__scmd_camera.pitch(-delta * __cameraRotationSpeed);
}

void PhongTestScene::__syncCameraExtent()
{
	_scmd_run([this]
	{
		float const displayHeight	{ static_cast<float>(__pDisplay->getHeight()) };
		float const displayWidth	{ static_cast<float>(__pDisplay->getWidth()) };

		float const cameraHeight	{ __scmd_camera.getNear() * glm::tan(__cameraFovy) };
		float const cameraWidth		{ cameraHeight * (displayWidth / displayHeight) };

		__scmd_camera.setHeight(cameraHeight);
		__scmd_camera.setWidth(cameraWidth);
	}).wait();
}

void PhongTestScene::__onDisplaySync()
{
	if (__pDisplay->isPresentable())
		__syncCameraExtent();
}