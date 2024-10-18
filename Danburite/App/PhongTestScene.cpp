#include "pch.h"
#include "PhongTestScene.h"
#include "../Frameworks/PrimitiveBuilder.h"
#include <glm/gtc/matrix_transform.hpp>

PhongTestScene::~PhongTestScene() noexcept
{
	_rcmd_run([this]
	{
		for (auto const &pLightMaterial : __rcmd_lightMaterials)
			_rcmd_removeGlobalMaterial(pLightMaterial.get());

		if (__pDisplay)
			__pDisplay->rcmd_getRenderTarget().removeLayer(__rcmd_pLayer.get());

		__rcmd_pLayer = nullptr;

		__rcmd_lightMaterials.clear();

		__rcmd_pContainerObject = nullptr;
		__rcmd_pContainerPhongMaterial = nullptr;

		for (auto &pMaterial : __rcmd_containerTransformMaterials)
			pMaterial = nullptr;

		__rcmd_pContainerTexture = nullptr;
		__rcmd_pContainerDrawParam = nullptr;
		__rcmd_pContainerMesh = nullptr;

		__rcmd_pPlaneObject = nullptr;
		__rcmd_pPlanePhongMaterial = nullptr;
		__rcmd_pPlaneTransformMaterial = nullptr;
		__rcmd_pPlaneTexture = nullptr;
		__rcmd_pPlaneDrawParam = nullptr;
		__rcmd_pPlaneMesh = nullptr;
	}).wait();
}

void PhongTestScene::setDisplay(
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

void PhongTestScene::syncDisplay()
{
	if (__pDisplay->isPresentable())
		__syncCameraExtent();
}

void PhongTestScene::addLight()
{
	_rcmd_silentRun([this]
	{
		auto pLightMaterial{ _rcmd_createMaterial<Frx::LightMaterial>() };

		auto const lightType{ __randomExt.nextBool() ? Frx::LightType::POINT : Frx::LightType::SPOT };

		pLightMaterial->setType(lightType);
		pLightMaterial->setColor(__randomExt.nextVec3(0.0f, 1.0f));

		if (lightType == Frx::LightType::POINT)
		{
			pLightMaterial->setPosition(__randomExt.nextVec3(-35.0f, 35.0f, 2.0f, 15.0f, -35.0f, 35.0f));
			pLightMaterial->setMaxDistance(64.0f);
			pLightMaterial->setAttenuation(1.0f, 0.14f, 0.07f);
		}
		else
		{
			pLightMaterial->setPosition(__randomExt.nextVec3(-30.0f, 30.0f, 35.0f, 50.0f, -30.0f, 30.0f));
			pLightMaterial->setMaxDistance(130.0f);
			pLightMaterial->setAttenuation(1.0f, 0.07f, 0.017f);

			auto const direction{ glm::normalize(__randomExt.nextVec3(-1.f, 1.f, -2.f, -1.f, -1.f, 1.f)) };
			pLightMaterial->setDirection(direction);
		}

		_rcmd_addGlobalMaterial(pLightMaterial);
		__rcmd_lightMaterials.emplace_back(pLightMaterial);

		__rcmd_lightUpdated = true;
	});
}

void PhongTestScene::removeLight()
{
	_rcmd_silentRun([this]
	{
		auto pLightMaterial{ std::move(__rcmd_lightMaterials.front()) };

		_rcmd_removeGlobalMaterial(pLightMaterial.get());
		__rcmd_lightMaterials.pop_front();

		__rcmd_lightUpdated = true;
	});
}

std::any PhongTestScene::_onInit()
{
	__camera.setPosition(0.0f, 5.0f, 10.0f);
	__camera.setNear(0.1f);
	__camera.validate();

	__UpdateParam retVal;
	retVal.containerTransforms.resize(__CONTAINER_COUNT);

	for (size_t containerIt{ }; containerIt < __CONTAINER_COUNT; ++containerIt)
	{
		auto &transformInfo	{ __containerTransforms[containerIt] };
		auto &transform		{ __containerTransforms[containerIt].transform };

		transformInfo.up				= glm::normalize(__randomExt.nextVec3(-1.0f, 1.0f));
		transformInfo.rotationSpeed		= __randomExt.nextFloat(0.1f, 2.0f);

		transform.getPosition().set(
			__randomExt.nextVec3(-40.0f, 40.0f, 1.0f, 20.0f, -40.0f, 40.0f));

		transform.validate();

		auto &matrix{ retVal.containerTransforms[containerIt] };
		matrix = transform.getMatrix();
	}

	retVal.cameraUpdated	= true;
	retVal.viewMatrix		= __camera.getViewMatrix();
	retVal.projMatrix		= __camera.getProjectionMatrix();
	retVal.cameraPos		= __camera.getPosition();

	return retVal;
}

std::any PhongTestScene::_onUpdate(
	Time const &time)
{
	float const delta{ static_cast<float>(time.deltaTime.count() * 1.0e-9) };
	__updateCamera(delta);

	__UpdateParam retVal;
	retVal.containerTransforms.resize(__CONTAINER_COUNT);

	for (size_t containerIt{ }; containerIt < __CONTAINER_COUNT; ++containerIt)
	{
		auto &transformInfo	{ __containerTransforms[containerIt] };
		auto &transform		{ __containerTransforms[containerIt].transform };
		auto &orientation	{ transform.getOrientation() };

		orientation.rotate(transformInfo.rotationSpeed * delta, transformInfo.up);
		transform.validate();

		auto &matrix{ retVal.containerTransforms[containerIt] };
		matrix = transform.getMatrix();
	}

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

void PhongTestScene::_rcmd_onInit(
	std::any const &initParam)
{
	auto const initData{ std::any_cast<__UpdateParam>(initParam) };

	for (size_t containerIt{ }; containerIt < __CONTAINER_COUNT; ++containerIt)
	{
		auto &pTransformMaterial{ __rcmd_containerTransformMaterials[containerIt] };
		pTransformMaterial = std::unique_ptr<Frx::TransformMaterial>{ _rcmd_createMaterial<Frx::TransformMaterial>() };

		auto const &transform{ initData.containerTransforms[containerIt] };
		pTransformMaterial->setTransform(transform);
	}

	__rcmd_globalData.viewMatrix = initData.viewMatrix;
	__rcmd_globalData.projMatrix = initData.projMatrix;
	__rcmd_globalData.cameraPos = initData.cameraPos;
	_rcmd_setGlobalData(__rcmd_globalData);

	__rcmd_pLayer = std::unique_ptr<Render::Layer>{ _rcmd_createLayer() };

	__rcmd_createPlaneObject();
	__rcmd_createContainerObject();

	__rcmd_pLayer->addRenderObject(__rcmd_pPlaneObject.get());
	__rcmd_pLayer->addRenderObject(__rcmd_pContainerObject.get());
}

void PhongTestScene::_rcmd_onUpdate(
	std::any const &updateParam)
{
	if (!__pDisplay)
		return;

	auto const param{ std::any_cast<__UpdateParam>(updateParam) };

	for (size_t containerIt{ }; containerIt < __CONTAINER_COUNT; ++containerIt)
	{
		auto &pTransformMaterial{ __rcmd_containerTransformMaterials[containerIt] };

		auto const &transform{ param.containerTransforms[containerIt] };
		pTransformMaterial->setTransform(transform);
	}

	if (param.cameraUpdated || __rcmd_lightUpdated)
	{
		if (param.cameraUpdated)
		{
			__rcmd_globalData.viewMatrix	= param.viewMatrix;
			__rcmd_globalData.projMatrix	= param.projMatrix;
			__rcmd_globalData.cameraPos		= param.cameraPos;
		}

		if (__rcmd_lightUpdated)
		{
			__rcmd_globalData.lightCount = static_cast<uint32_t>(__rcmd_lightMaterials.size());

			uint32_t slotIdx{ };
			for (auto const &pLightMaterial : __rcmd_lightMaterials)
			{
				__rcmd_globalData.lightIndices[slotIdx] =
					_rcmd_getGlobalMaterialIdOf(pLightMaterial.get());

				++slotIdx;
			}

			__rcmd_lightUpdated = false;
		}

		_rcmd_setGlobalData(__rcmd_globalData);
	}

	auto &renderTarget{ __pDisplay->rcmd_getRenderTarget() };
	renderTarget.requestRedraw();
}

void PhongTestScene::__updateCamera(
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

void PhongTestScene::__rcmd_createPlaneObject()
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

	__rcmd_pPlanePhongMaterial->setTexture(Frx::TextureType::DIFFUSE, 0U, __rcmd_pPlaneTexture.get());
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

void PhongTestScene::__rcmd_createContainerObject()
{
	auto const meshData
	{
		Frx::PrimitiveBuilder::buildCube(
			Frx::VertexAttribFlagBits::POS |
			Frx::VertexAttribFlagBits::NORMAL |
			Frx::VertexAttribFlagBits::COLOR |
			Frx::VertexAttribFlagBits::UV0, 1.0f)
	};

	__rcmd_pContainerMesh = std::unique_ptr<Render::Mesh>{ _rcmd_createMesh() };
	__rcmd_pContainerMesh->createVertexBuffer(Frx::VertexAttrib::POS_LOCATION, meshData.posBuffer.getData(), meshData.posBuffer.getSize());
	__rcmd_pContainerMesh->createVertexBuffer(Frx::VertexAttrib::NORMAL_LOCATION, meshData.normalBuffer.getData(), meshData.normalBuffer.getSize());
	__rcmd_pContainerMesh->createVertexBuffer(Frx::VertexAttrib::COLOR_LOCATION, meshData.colorBuffer.getData(), meshData.colorBuffer.getSize());
	__rcmd_pContainerMesh->createVertexBuffer(Frx::VertexAttrib::UV_LOCATIONS[0], meshData.uvBuffer.getData(), meshData.uvBuffer.getSize());
	__rcmd_pContainerMesh->createIndexBuffer(meshData.indexType, meshData.indexBuffer.getData(), meshData.indexBuffer.getSize());

	__rcmd_pContainerDrawParam = std::make_unique<Render::DrawParamIndexed>(meshData.indexCount, 0U, 0);
	__rcmd_pContainerTexture = std::unique_ptr<Render::Texture>
	{
		_rcmd_createTexture(
			"Images/container.png", true,
			VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
			VK_ACCESS_2_SHADER_SAMPLED_READ_BIT)
	};

	__rcmd_pContainerPhongMaterial =
		std::unique_ptr<Frx::PhongMaterial>{ _rcmd_createMaterial<Frx::PhongMaterial>() };

	__rcmd_pContainerPhongMaterial->setTexture(Frx::TextureType::DIFFUSE, 0U, __rcmd_pContainerTexture.get());
	__rcmd_pContainerPhongMaterial->setShininess(32.0f);
	__rcmd_pContainerPhongMaterial->setOpacity(0.0f);

	__rcmd_pContainerObject = std::make_unique<Render::RenderObject>();
	__rcmd_pContainerObject->setMesh(__rcmd_pContainerMesh.get());
	__rcmd_pContainerObject->setDrawParam(__rcmd_pContainerDrawParam.get());
	__rcmd_pContainerObject->setRenderer(_rcmd_getRendererOf(Frx::RendererType::PHONG));
	__rcmd_pContainerObject->setInstanceCount(__CONTAINER_COUNT);

	for (uint32_t objIter{ }; objIter < __CONTAINER_COUNT; ++objIter)
	{
		auto &materialPack{ __rcmd_pContainerObject->getMaterialPackOf(objIter) };
		materialPack.setMaterial(__rcmd_containerTransformMaterials[objIter].get());
		materialPack.setMaterial(__rcmd_pContainerPhongMaterial.get());
	}
}