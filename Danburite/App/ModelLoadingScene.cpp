#include "pch.h"
#include "ModelLoadingScene.h"
#include "../Frameworks/PrimitiveBuilder.h"
#include "../Frameworks/Model.h"
#include <atomic>

ModelLoadingScene::~ModelLoadingScene() noexcept
{
	_rcmd_run([this]
	{
		for (auto const &pLightMaterial : __rcmd_lightMaterials)
			_rcmd_removeGlobalMaterial(pLightMaterial.get());

		if (__pDisplay)
			__pDisplay->rcmd_getRenderTarget().removeLayer(__rcmd_pLayer.get());

		__rcmd_pLayer = nullptr;

		__rcmd_lightMaterials.clear();

		__rcmd_pPlaneObject = nullptr;
		__rcmd_pPlanePhongMaterial = nullptr;
		__rcmd_pPlaneTransformMaterial = nullptr;
		__rcmd_pPlaneTexture = nullptr;
		__rcmd_pPlaneDrawParam = nullptr;
		__rcmd_pPlaneMesh = nullptr;
	}).wait();

	__pModel = nullptr;
}

void ModelLoadingScene::setDisplay(
	Frx::Display *const pDisplay)
{
	if (__pDisplay == pDisplay)
		return;

	auto const pPrevDisplay{ __pDisplay };
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

	__pDisplay = pDisplay;

	if (__pDisplay)
		syncDisplay();
}

void ModelLoadingScene::syncDisplay()
{
	if (__pDisplay->isPresentable())
		__syncCameraExtent();
}

std::any ModelLoadingScene::_onInit()
{
	_loadModel(R"(Models\backpack\backpack.obj)");

	__camera.setPosition(0.0f, 5.0f, 10.0f);
	__camera.setNear(0.1f);
	__camera.validate();

	__UpdateParam retVal;

	retVal.cameraUpdated	= true;
	retVal.viewMatrix		= __camera.getViewMatrix();
	retVal.projMatrix		= __camera.getProjectionMatrix();
	retVal.cameraPos		= __camera.getPosition();

	return retVal;
}

std::any ModelLoadingScene::_onUpdate(
	Time const &time)
{
	float const delta{ static_cast<float>(time.deltaTime.count() * 1.0e-9) };
	__updateCamera(delta);

	__UpdateParam retVal;

	if (__camera.isInvalidated())
	{
		__camera.validate();
		retVal.cameraUpdated = true;
		retVal.viewMatrix	= __camera.getViewMatrix();
		retVal.projMatrix	= __camera.getProjectionMatrix();
		retVal.cameraPos	= __camera.getPosition();;
	}

	return retVal;
}

void ModelLoadingScene::_onModelLoaded(
	uint32_t requestIdx,
	Frx::Model::CreateInfo &&result)
{
	__pModel = std::unique_ptr<Frx::Model>{ _createModel(std::move(result)) };
	std::atomic_thread_fence(std::memory_order::release);

	_rcmd_silentRun([this]
	{
		__pModel->rcmd_addToLayer(*__rcmd_pLayer);
	});
}

void ModelLoadingScene::_rcmd_onInit(
	std::any const &initParam)
{
	auto const initData{ std::any_cast<__UpdateParam>(initParam) };

	for (uint32_t lightIter{ }; lightIter < __MAX_LIGHT_COUNT; ++lightIter)
		__rcmd_addLight();

	__rcmd_globalData.viewMatrix	= initData.viewMatrix;
	__rcmd_globalData.projMatrix	= initData.projMatrix;
	__rcmd_globalData.cameraPos		= initData.cameraPos;
	__rcmd_globalData.lightCount	= static_cast<uint32_t>(__rcmd_lightMaterials.size());

	uint32_t slotIdx{ };
	for (auto const &pLightMaterial : __rcmd_lightMaterials)
	{
		__rcmd_globalData.lightIndices[slotIdx] =
			_rcmd_getGlobalMaterialIdOf(pLightMaterial.get());

		++slotIdx;
	}

	_rcmd_setGlobalData(__rcmd_globalData);
	__rcmd_pLayer = std::unique_ptr<Render::Layer>{ _rcmd_createLayer() };

	__rcmd_createPlaneObject();

	__rcmd_pLayer->addRenderObject(__rcmd_pPlaneObject.get());
}

void ModelLoadingScene::_rcmd_onUpdate(
	std::any const &updateParam)
{
	if (!__pDisplay)
		return;

	auto const param{ std::any_cast<__UpdateParam>(updateParam) };

	if (param.cameraUpdated)
	{
		if (param.cameraUpdated)
		{
			__rcmd_globalData.viewMatrix	= param.viewMatrix;
			__rcmd_globalData.projMatrix	= param.projMatrix;
			__rcmd_globalData.cameraPos		= param.cameraPos;
		}

		_rcmd_setGlobalData(__rcmd_globalData);
	}

	auto &renderTarget{ __pDisplay->rcmd_getRenderTarget() };
	renderTarget.requestRedraw();
}

void ModelLoadingScene::__updateCamera(
	float const delta)
{
	float cameraMoveSpeed{ __cameraMoveSpeed };

	if (__cameraMoveAccelerated)
		cameraMoveSpeed *= 3.0f;

	if (__cameraMoveRight)
		__camera.moveLocalX(delta * cameraMoveSpeed);

	if (__cameraMoveLeft)
		__camera.moveLocalX(-delta * cameraMoveSpeed);

	if (__cameraMoveUp)
		__camera.moveLocalY(delta * cameraMoveSpeed);

	if (__cameraMoveDown)
		__camera.moveLocalY(-delta * cameraMoveSpeed);

	if (__cameraMoveForward)
		__camera.moveLocalZ(-delta * cameraMoveSpeed);

	if (__cameraMoveBackward)
		__camera.moveLocalZ(delta * cameraMoveSpeed);

	if (__cameraRotateRight)
		__camera.yaw(-delta * __cameraRotationSpeed);

	if (__cameraRotateLeft)
		__camera.yaw(delta * __cameraRotationSpeed);

	if (__cameraRotateUp)
		__camera.pitch(delta * __cameraRotationSpeed);

	if (__cameraRotateDown)
		__camera.pitch(-delta * __cameraRotationSpeed);
}

void ModelLoadingScene::__syncCameraExtent()
{
	float const displayHeight	{ static_cast<float>(__pDisplay->getHeight()) };
	float const displayWidth	{ static_cast<float>(__pDisplay->getWidth()) };

	float const cameraHeight	{ __camera.getNear() * glm::tan(__cameraFovy) };
	float const cameraWidth		{ cameraHeight * (displayWidth / displayHeight) };

	__camera.setHeight(cameraHeight);
	__camera.setWidth(cameraWidth);
}

void ModelLoadingScene::__onDisplaySync()
{
	syncDisplay();
}

void ModelLoadingScene::__rcmd_createPlaneObject()
{
	auto const meshData
	{
		Frx::PrimitiveBuilder::buildSquare(
			Frx::VertexAttribFlagBits::POS |
			Frx::VertexAttribFlagBits::NORMAL |
			Frx::VertexAttribFlagBits::COLOR |
			Frx::VertexAttribFlagBits::UV0, 100.0f, 10.0f)
	};

	__rcmd_pPlaneMesh = std::unique_ptr<Render::Mesh>{ _rcmd_createMesh() };
	__rcmd_pPlaneMesh->createVertexBuffer(Frx::VertexAttrib::POS_LOCATION, meshData.posBuffer.getData(), meshData.posBuffer.getSize());
	__rcmd_pPlaneMesh->createVertexBuffer(Frx::VertexAttrib::NORMAL_LOCATION, meshData.normalBuffer.getData(), meshData.normalBuffer.getSize());
	__rcmd_pPlaneMesh->createVertexBuffer(Frx::VertexAttrib::COLOR_LOCATION, meshData.colorBuffer.getData(), meshData.colorBuffer.getSize());
	__rcmd_pPlaneMesh->createVertexBuffer(Frx::VertexAttrib::UV_LOCATIONS[0], meshData.uvBuffer.getData(), meshData.uvBuffer.getSize());
	__rcmd_pPlaneMesh->createIndexBuffer(meshData.indexType, meshData.indexBuffer.getData(), meshData.indexBuffer.getSize());

	__rcmd_pPlaneDrawParam = std::make_unique<Render::DrawParamIndexed>(meshData.indexCount, 0U, 0);

	__rcmd_pPlaneTexture = std::unique_ptr<Render::Texture>
	{
		_rcmd_createTexture(
			"Images/wood.jpg", true,
			VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
			VK_ACCESS_2_SHADER_SAMPLED_READ_BIT)
	};

	__rcmd_pPlaneTransformMaterial =
		std::unique_ptr<Frx::TransformMaterial>{ _rcmd_createMaterial<Frx::TransformMaterial>() };

	glm::mat4 transform{ 1.0f };
	transform = glm::rotate(transform, -glm::half_pi<float>(), glm::vec3{ 1.0f, 0.0f, 0.0f });
	__rcmd_pPlaneTransformMaterial->setTransform(transform);

	__rcmd_pPlanePhongMaterial =
		std::unique_ptr<Frx::PhongMaterial>{ _rcmd_createMaterial<Frx::PhongMaterial>() };

	__rcmd_pPlanePhongMaterial->setTexture(Frx::TextureType::AMBIENT, 0U, __rcmd_pPlaneTexture.get());
	__rcmd_pPlanePhongMaterial->setTexture(Frx::TextureType::DIFFUSE, 0U, __rcmd_pPlaneTexture.get());
	__rcmd_pPlanePhongMaterial->setTexture(Frx::TextureType::SPECULAR, 0U, __rcmd_pPlaneTexture.get());
	__rcmd_pPlanePhongMaterial->setShininess(32.0f);
	__rcmd_pPlanePhongMaterial->setOpacity(0.0f);

	__rcmd_pPlaneObject = std::make_unique<Render::RenderObject>();
	__rcmd_pPlaneObject->setMesh(__rcmd_pPlaneMesh.get());
	__rcmd_pPlaneObject->setDrawParam(__rcmd_pPlaneDrawParam.get());
	__rcmd_pPlaneObject->setRenderer(_rcmd_getRendererOf(Frx::RendererType::PHONG));

	auto &materialPack{ __rcmd_pPlaneObject->getMaterialPackOf(0U) };
	materialPack.setMaterial(__rcmd_pPlaneTransformMaterial.get());
	materialPack.setMaterial(__rcmd_pPlanePhongMaterial.get());
}

void ModelLoadingScene::__rcmd_addLight()
{
	auto pLightMaterial{ _rcmd_createMaterial<Frx::LightMaterial>() };
	pLightMaterial->setType(Frx::LightType::POINT);
	pLightMaterial->setColor(__randomExt.nextVec3(0.0f, 1.0f));
	pLightMaterial->setPosition(__randomExt.nextVec3(-35.0f, 35.0f, 2.0f, 15.0f, -35.0f, 35.0f));
	pLightMaterial->setMaxDistance(130.0f);
	pLightMaterial->setAttenuation(1.0f, 0.07f, 0.017f);

	_rcmd_addGlobalMaterial(pLightMaterial);
	__rcmd_lightMaterials.emplace_back(pLightMaterial);
}