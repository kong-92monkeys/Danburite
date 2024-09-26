#include "pch.h"
#include "TestScene.h"
#include "../Frameworks/Vertex.h"
#include "../Render/TextureUtil.h"

TestScene::~TestScene() noexcept
{
	_getRcmdExecutor().run([this]
	{
		if (__rcmd_pDisplay)
			__rcmd_pDisplay->rcmd_getRenderTarget().removeLayer(__rcmd_pLayer.get());

		__rcmd_pLayer = nullptr;
		__rcmd_pObject = nullptr;
		__rcmd_pMaterial = nullptr;
		__rcmd_pTexture = nullptr;
		__rcmd_pRenderer = nullptr;
		__rcmd_pDrawParam = nullptr;
		__rcmd_pMesh = nullptr;
	}).wait();
}

void TestScene::setDisplay(
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

void TestScene::_scmd_onInit()
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

		Infra::GenericBuffer indexBuffer;
		indexBuffer.typedAdd<uint16_t>({ 0U, 1U, 2U, 0U, 2U, 3U });

		__rcmd_pMesh = std::unique_ptr<Render::Mesh>{ renderEngine.createMesh() };
		__rcmd_pMesh->createVertexBuffer(Frx::VertexAttrib::POS_LOCATION, posBuffer.getData(), posBuffer.getSize());
		__rcmd_pMesh->createVertexBuffer(Frx::VertexAttrib::UV_LOCATION, uvBuffer.getData(), uvBuffer.getSize());
		__rcmd_pMesh->createIndexBuffer(VkIndexType::VK_INDEX_TYPE_UINT16, indexBuffer.getData(), indexBuffer.getSize());

		__rcmd_pDrawParam = std::make_unique<Render::DrawParamIndexed>(6U, 0U, 0);
		__rcmd_pRenderer = std::unique_ptr<Frx::ImageRenderer>{ renderEngine.createRenderer<Frx::ImageRenderer>() };

		__rcmd_pTexture = std::unique_ptr<Render::Texture>
		{
			Render::TextureUtil::loadTexture(
				renderEngine, "Images/smile.jpg",
				VK_PIPELINE_STAGE_2_NONE, VK_ACCESS_2_NONE,
				VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT, VK_ACCESS_2_SHADER_SAMPLED_READ_BIT)
		};

		__rcmd_pMaterial = std::unique_ptr<Frx::ImageMaterial>{ renderEngine.createMaterial<Frx::ImageMaterial>() };
		__rcmd_pMaterial->setTexture(__rcmd_pTexture.get());

		__rcmd_pObject = std::make_unique<Render::RenderObject>();
		__rcmd_pObject->setMesh(__rcmd_pMesh.get());
		__rcmd_pObject->setDrawParam(__rcmd_pDrawParam.get());
		__rcmd_pObject->setRenderer(__rcmd_pRenderer.get());
		__rcmd_pObject->getMaterialPackOf(0U).setMaterial(__rcmd_pMaterial.get());

		__rcmd_pLayer = std::unique_ptr<Render::Layer>{ renderEngine.createLayer() };
		__rcmd_pLayer->addRenderObject(__rcmd_pObject.get());
	});
}