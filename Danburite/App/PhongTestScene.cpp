#include "pch.h"
#include "PhongTestScene.h"
#include "../Frameworks/PrimitiveBuilder.h"
#include <glm/gtc/matrix_transform.hpp>

PhongTestScene::~PhongTestScene() noexcept
{
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

	if (__pDisplay)
		syncDisplay();

	_rcmd_run([this, pPrevDisplay, pDisplay]
	{
		if (pPrevDisplay)
			pPrevDisplay->rcmd_getRenderTarget().removeLayer(__rcmd_pLayer.get());

		if (pDisplay)
		{
			pDisplay->rcmd_getRenderTarget().setClearColor(glm::vec4{ 0.007f, 0.007f, 0.007f, 1.0f });
			pDisplay->rcmd_getRenderTarget().addLayer(__rcmd_pLayer.get());
		}
	}).wait();
}

void PhongTestScene::syncDisplay()
{
	if (__pDisplay->isPresentable())
		__syncCameraExtent();
}

std::any PhongTestScene::_onInit()
{
	__camera.setPosition(0.0f, 0.0f, 3.0f);
	__camera.setNear(0.1f);
	__camera.validate();

	return __GlobalData
	{
		.viewMatrix{ __camera.getViewMatrix() },
		.projMatrix{ __camera.getProjectionMatrix() }
	};
}

std::any PhongTestScene::_onUpdate(
	Time const &time)
{
	float const delta{ static_cast<float>(time.deltaTime.count() * 1.0e-9) };

	__objectTransform.getOrientation().rotate(
		delta * __objectRotationSpeed, glm::vec3{ 0.0f, 1.0f, 0.0f });

	__handleCamera(delta);

	__objectTransform.validate();

	__UpdateParam retVal;
	retVal.objectTransform = __objectTransform.getMatrix();

	if (__camera.isInvalidated())
	{
		__camera.validate();
		retVal.globalDataUpdated = true;
		retVal.globalData.viewMatrix = __camera.getViewMatrix();
		retVal.globalData.projMatrix = __camera.getProjectionMatrix();
		retVal.globalData.cameraPosition = __camera.getPosition();;
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
	__rcmd_pLightMaterial->setType(Frx::LightType::POINT);
	__rcmd_pLightMaterial->setColor(glm::vec4{ 1.0f, 0.5f, 0.0f, 1.0f });
	__rcmd_pLightMaterial->setPosition(glm::vec3{ -1.0f, 1.0f, 1.0f });
	__rcmd_pLightMaterial->setDirection(glm::normalize(glm::vec3{ 2.0f, -3.0f, -1.0f }));

	_rcmd_addGlobalMaterial(__rcmd_pLightMaterial.get());
	__rcmd_lightIdx = static_cast<int>(_rcmd_getGlobalMaterialIdOf(__rcmd_pLightMaterial.get()));

	auto const globalData{ std::any_cast<__GlobalData>(initParam) };
	_rcmd_setGlobalData(globalData);
}

void PhongTestScene::_rcmd_onUpdate(
	std::any const &updateParam)
{
	if (!__pDisplay)
		return;

	auto param{ std::any_cast<__UpdateParam>(updateParam) };
	__rcmd_pTransformMaterial->setTransform(param.objectTransform);

	if (param.globalDataUpdated)
	{
		param.globalData.lightIdx = __rcmd_lightIdx;
		_rcmd_setGlobalData(param.globalData);
	}

	auto &renderTarget{ __pDisplay->rcmd_getRenderTarget() };
	renderTarget.requestRedraw();
}

void PhongTestScene::__handleCamera(
	float const delta)
{
	if (__cameraMoveRight)
		__camera.moveLocalX(delta * __cameraMoveSpeed);

	if (__cameraMoveLeft)
		__camera.moveLocalX(-delta * __cameraMoveSpeed);

	if (__cameraMoveUp)
		__camera.moveLocalY(delta * __cameraMoveSpeed);

	if (__cameraMoveDown)
		__camera.moveLocalY(-delta * __cameraMoveSpeed);

	if (__cameraMoveForward)
		__camera.moveLocalZ(-delta * __cameraMoveSpeed);

	if (__cameraMoveBackward)
		__camera.moveLocalZ(delta * __cameraMoveSpeed);

	if (__cameraRotateRight)
		__camera.yaw(-delta * __cameraRotationSpeed);

	if (__cameraRotateLeft)
		__camera.yaw(delta * __cameraRotationSpeed);

	if (__cameraRotateUp)
		__camera.pitch(delta * __cameraRotationSpeed);

	if (__cameraRotateDown)
		__camera.pitch(-delta * __cameraRotationSpeed);
}

void PhongTestScene::__syncCameraExtent()
{
	float const displayHeight	{ static_cast<float>(__pDisplay->getHeight()) };
	float const displayWidth	{ static_cast<float>(__pDisplay->getWidth()) };

	float const cameraHeight	{ __camera.getNear() * glm::tan(__cameraFovy) };
	float const cameraWidth		{ cameraHeight * (displayWidth / displayHeight) };

	__camera.setHeight(cameraHeight);
	__camera.setWidth(cameraWidth);
}

void PhongTestScene::__onDisplaySync()
{
	syncDisplay();
}