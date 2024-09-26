#include "pch.h"
#include "PhongTestScene.h"
#include "../Frameworks/Vertex.h"
#include "../Render/TextureUtil.h"
#include <glm/gtc/matrix_transform.hpp>

PhongTestScene::~PhongTestScene() noexcept
{
	_getRcmdExecutor().run([this]
	{
		if (__rcmd_pDisplay)
			__rcmd_pDisplay->rcmd_getRenderTarget().removeLayer(__rcmd_pLayer.get());

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
	_getRcmdExecutor().run([this, pDisplay]
	{
		if (__rcmd_pDisplay == pDisplay)
			return;

		auto const pPrevDisplay{ __rcmd_pDisplay };
		__rcmd_pDisplay = pDisplay;

		if (pPrevDisplay)
			pPrevDisplay->rcmd_getRenderTarget().removeLayer(__rcmd_pLayer.get());

		if (pDisplay)
			pDisplay->rcmd_getRenderTarget().addLayer(__rcmd_pLayer.get());

	}).wait();
}

void PhongTestScene::_scmd_onInit()
{
	_getRcmdExecutor().silentRun([this]
	{
		auto &renderEngine{ _getRenderEngine() };

		Infra::GenericBuffer posBuffer;
		posBuffer.typedAdd<glm::vec3>({ -0.5f, -0.5f, 0.5f });
		posBuffer.typedAdd<glm::vec3>({ -0.5f, 0.5f, 0.5f });
		posBuffer.typedAdd<glm::vec3>({ 0.5f, 0.5f, 0.5f });
		posBuffer.typedAdd<glm::vec3>({ 0.5f, -0.5f, 0.5f });

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

		__rcmd_pMesh = std::unique_ptr<Render::Mesh>{ renderEngine.createMesh() };
		__rcmd_pMesh->createVertexBuffer(Frx::VertexAttrib::POS_LOCATION, posBuffer.getData(), posBuffer.getSize());
		__rcmd_pMesh->createVertexBuffer(Frx::VertexAttrib::UV_LOCATION, uvBuffer.getData(), uvBuffer.getSize());
		__rcmd_pMesh->createVertexBuffer(Frx::VertexAttrib::COLOR_LOCATION, colorBuffer.getData(), colorBuffer.getSize());
		__rcmd_pMesh->createIndexBuffer(VkIndexType::VK_INDEX_TYPE_UINT16, indexBuffer.getData(), indexBuffer.getSize());

		__rcmd_pDrawParam = std::make_unique<Render::DrawParamIndexed>(6U, 0U, 0);
		__rcmd_pRenderer = std::unique_ptr<Frx::PhongRenderer>{ renderEngine.createRenderer<Frx::PhongRenderer>() };

		__rcmd_pAlbedoTexture = std::unique_ptr<Render::Texture>
		{
			Render::TextureUtil::loadTexture(
				renderEngine, "Images/smile.jpg",
				VK_PIPELINE_STAGE_2_NONE, VK_ACCESS_2_NONE,
				VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT, VK_ACCESS_2_SHADER_SAMPLED_READ_BIT)
		};

		__rcmd_pTransformMaterial = std::unique_ptr<Frx::TransformMaterial>{ renderEngine.createMaterial<Frx::TransformMaterial>() };
		__rcmd_pTransformMaterial->setTransform(glm::rotate(glm::mat4{ 1.0f }, glm::quarter_pi<float>(), glm::vec3{ 0.0f, 0.0f, 1.0f }));

		__rcmd_pPhongMaterial = std::unique_ptr<Frx::PhongMaterial>{ renderEngine.createMaterial<Frx::PhongMaterial>() };
		__rcmd_pPhongMaterial->setAlbedoTexture(__rcmd_pAlbedoTexture.get());

		__rcmd_pObject = std::make_unique<Render::RenderObject>();
		__rcmd_pObject->setMesh(__rcmd_pMesh.get());
		__rcmd_pObject->setDrawParam(__rcmd_pDrawParam.get());
		__rcmd_pObject->setRenderer(__rcmd_pRenderer.get());
		__rcmd_pObject->getMaterialPackOf(0U).setMaterial(__rcmd_pTransformMaterial.get());
		__rcmd_pObject->getMaterialPackOf(0U).setMaterial(__rcmd_pPhongMaterial.get());

		__rcmd_pLayer = std::unique_ptr<Render::Layer>{ renderEngine.createLayer() };
		__rcmd_pLayer->addRenderObject(__rcmd_pObject.get());
	});
}