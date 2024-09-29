#include "pch.h"
#include "PhongTestScene.h"
#include "../Frameworks/Vertex.h"
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
		if (__pDisplay)
			__pDisplay->rcmd_getRenderTarget().removeLayer(__rcmd_pLayer.get());

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

void PhongTestScene::_scmd_onInit()
{
	__scmd_camera.getTransform().getPosition().setZ(10.0f);
}

std::any PhongTestScene::_scmd_onUpdate(
	Time const &time)
{
	float const delta{ static_cast<float>(time.deltaTime.count() * 1.0e-9) };

	auto &cameraTransform	{ __scmd_camera.getTransform() };
	auto &cameraPosition	{ cameraTransform.getPosition() };

	if (__cameraMoveRight)
		cameraPosition.addX(delta * __cameraMoveSpeed);

	if (__cameraMoveLeft)
		cameraPosition.addX(-delta * __cameraMoveSpeed);

	if (__cameraMoveUp)
		cameraPosition.addY(delta * __cameraMoveSpeed);

	if (__cameraMoveDown)
		cameraPosition.addY(-delta * __cameraMoveSpeed);

	if (__cameraMoveForward)
		cameraPosition.addZ(-delta * __cameraMoveSpeed);

	if (__cameraMoveBackward)
		cameraPosition.addZ(delta * __cameraMoveSpeed);

	__scmd_camera.validate();

	return __LayerData
	{
		.viewMatrix{ __scmd_camera.getViewMatrix() },
		.projMatrix{ __scmd_camera.getProjectionMatrix() }
	};
}

void PhongTestScene::_rcmd_onInit()
{
	Infra::GenericBuffer posBuffer;
	posBuffer.typedAdd<glm::vec3>({ -0.5f, 0.5f, 0.0f });
	posBuffer.typedAdd<glm::vec3>({ -0.5f, -0.5f, 0.0f });
	posBuffer.typedAdd<glm::vec3>({ 0.5f, -0.5f, 0.0f });
	posBuffer.typedAdd<glm::vec3>({ 0.5f, 0.5f, 0.0f });

	Infra::GenericBuffer uvBuffer;
	uvBuffer.typedAdd<glm::vec2>({ 0.0f, 0.0f });
	uvBuffer.typedAdd<glm::vec2>({ 0.0f, 1.0f });
	uvBuffer.typedAdd<glm::vec2>({ 1.0f, 1.0f });
	uvBuffer.typedAdd<glm::vec2>({ 1.0f, 0.0f });

	Infra::GenericBuffer colorBuffer;
	colorBuffer.typedAdd<glm::vec4>({ 1.0f, 0.0f, 0.0f, 1.0f });
	colorBuffer.typedAdd<glm::vec4>({ 0.0f, 1.0f, 0.0f, 1.0f });
	colorBuffer.typedAdd<glm::vec4>({ 0.0f, 0.0f, 1.0f, 1.0f });
	colorBuffer.typedAdd<glm::vec4>({ 1.0f, 1.0f, 0.0f, 1.0f });

	Infra::GenericBuffer indexBuffer;
	indexBuffer.typedAdd<uint16_t>({ 0U, 1U, 2U, 0U, 2U, 3U });

	__rcmd_pMesh = _rcmd_createMesh();
	__rcmd_pMesh->createVertexBuffer(Frx::VertexAttrib::POS_LOCATION, posBuffer.getData(), posBuffer.getSize());
	__rcmd_pMesh->createVertexBuffer(Frx::VertexAttrib::UV_LOCATION, uvBuffer.getData(), uvBuffer.getSize());
	__rcmd_pMesh->createVertexBuffer(Frx::VertexAttrib::COLOR_LOCATION, colorBuffer.getData(), colorBuffer.getSize());
	__rcmd_pMesh->createIndexBuffer(VkIndexType::VK_INDEX_TYPE_UINT16, indexBuffer.getData(), indexBuffer.getSize());

	__rcmd_pDrawParam = std::make_unique<Render::DrawParamIndexed>(6U, 0U, 0);
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
	__rcmd_pLayer->setData(__LayerData{ });
	__rcmd_pLayer->addRenderObject(__rcmd_pObject.get());
}

void PhongTestScene::_rcmd_onUpdate(
	std::any const &updateParam)
{
	auto const layerData{ std::any_cast<__LayerData>(updateParam) };
	__rcmd_pLayer->setData(layerData);
}

void PhongTestScene::__syncCameraExtent()
{
	_scmd_run([this]
	{
		float const displayHeight	{ static_cast<float>(__pDisplay->getHeight()) };
		float const displayWidth	{ static_cast<float>(__pDisplay->getWidth()) };

		float const cameraHeight	{ 2.0f };
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