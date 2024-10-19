#include "Model.h"
#include "TextureUtil.h"

namespace Frx
{
	Model::Model(
		CreateInfo &&createInfo,
		Infra::Executor &rcmdExecutor,
		Render::Engine &renderEngine,
		RendererFactory &rendererFactory) :
		__rcmdExecutor	{ rcmdExecutor }
	{
		__pRootNodeInvalidateListener =
			Infra::EventListener<SceneNode *>::bind(
				&Model::__onRootNodeInvalidated, this);

		__rootNode.getInvalidateEvent() += __pRootNodeInvalidateListener;

		size_t const nodeCount{ createInfo.nodes.size() };
		__sceneNodes.resize(nodeCount);

		for (size_t nodeIter{ nodeCount - 1ULL }; ; --nodeIter)
		{
			auto const &nodeInfo{ createInfo.nodes[nodeIter] };

			auto &pNode{ __sceneNodes[nodeIter] };
			pNode = new SceneNode{ nodeInfo.transform };

			for (size_t const childIdx : nodeInfo.childIndices)
				pNode->addChild(__sceneNodes[childIdx]);

			if (!nodeIter)
				break;
		}

		__rootNode.addChild(__sceneNodes.front());

		__rcmdExecutor.silentRun([
			&renderEngine, &rendererFactory,
			createInfo{ std::move(createInfo) },
			pRcmdResources{ __pRcmdResources }]
		{
			__rcmd_init(createInfo, renderEngine, rendererFactory, *pRcmdResources);
		});
	}

	Model::~Model() noexcept
	{
		for (auto const pNode : __sceneNodes)
			delete pNode;

		__rcmdExecutor.silentRun([pRcmdResources{ __pRcmdResources }]
		{
			delete pRcmdResources;
		});
	}

	void Model::rcmd_addToLayer(
		Render::Layer &layer)
	{
		for (auto const &pObject : __pRcmdResources->renderObjects)
			layer.addRenderObject(pObject.get());
	}

	void Model::rcmd_removeFromLayer(
		Render::Layer &layer)
	{
		for (auto const &pObject : __pRcmdResources->renderObjects)
			layer.removeRenderObject(pObject.get());
	}

	void Model::_onValidate()
	{
		if (__nodeTransformInvalidated)
			__validateNodeTransforms();

		__nodeTransformInvalidated = false;
	}

	void Model::__validateNodeTransforms()
	{
		__rootNode.validate();

		std::vector<glm::mat4> nodeTransforms;

		size_t const nodeCount{ __sceneNodes.size() };
		nodeTransforms.resize(nodeCount);

		for (size_t nodeIt{ }; nodeIt < nodeCount; ++nodeIt)
			nodeTransforms[nodeIt] = __sceneNodes[nodeIt]->getGlobalTransform();

		__rcmdExecutor.silentRun([
			pRcmdResources{ __pRcmdResources },
			nodeTransforms{ std::move(nodeTransforms) }]
		{
			__rcmd_updateNodeTransforms(*pRcmdResources, nodeTransforms);
		});
	}

	void Model::__onRootNodeInvalidated()
	{
		__nodeTransformInvalidated = true;
		_invalidate();
	}

	void Model::__rcmd_init(
		CreateInfo const &createInfo,
		Render::Engine &renderEngine,
		RendererFactory &rendererFactory,
		__RcmdResources &outResources)
	{
		auto &textures				{ outResources.textures };
		auto &materials				{ outResources.materials };
		auto &meshes				{ outResources.meshes };
		auto &drawParams			{ outResources.drawParams };

		auto &renderObjects			{ outResources.renderObjects };
		auto &transformMaterials	{ outResources.transformMaterials };

		// Textures
		{
			size_t const texCount{ createInfo.textures.size() };
			for (size_t texIter{ }; texIter < texCount; ++texIter)
			{
				auto const &bitmap{ *(createInfo.textures[texIter]) };

				auto const pTexture
				{
					TextureUtil::loadTexture(
						renderEngine, bitmap, true,
						VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
						VK_ACCESS_2_SHADER_SAMPLED_READ_BIT)
				};

				textures.emplace_back(pTexture);
			}
		}

		// Materials
		{
			size_t const materialCount{ createInfo.materials.size() };
			for (size_t materialIter{ }; materialIter < materialCount; ++materialIter)
			{
				auto const &materialInfo{ createInfo.materials[materialIter] };
				switch (materialInfo.rendererType)
				{
					case RendererType::PHONG:
					{
						auto const pMaterial{ __rcmd_createPhongMaterial(renderEngine, textures, materialInfo) };
						materials.emplace_back(pMaterial);
					}
						break;
				}
			}
		}
		
		// Meshes
		{
			size_t const meshCount{ createInfo.meshes.size() };
			for (size_t meshIt{ }; meshIt < meshCount; ++meshIt)
			{
				auto const &meshInfo	{ createInfo.meshes[meshIt] };
				auto const pMesh		{ renderEngine.createMesh() };

				for (auto const &[binding, buffer] : meshInfo.vertexBuffers)
					pMesh->createVertexBuffer(binding, buffer.getData(), buffer.getSize());

				pMesh->createIndexBuffer(
					meshInfo.indexType,
					meshInfo.indexBuffer.getData(), meshInfo.indexBuffer.getSize());

				meshes.emplace_back(pMesh);
			}
		}
		
		// Draw params
		{
			size_t const drawParamCount{ createInfo.drawInfos.size() };
			for (size_t drawParamIt{ }; drawParamIt < drawParamCount; ++drawParamIt)
			{
				auto const &drawInfo{ createInfo.drawInfos[drawParamIt] };

				drawParams.emplace_back(
					std::make_shared<Render::DrawParamIndexed>(
						drawInfo.indexCount, drawInfo.firstIndex, drawInfo.vertexOffset));
			}
		}

		// Render objects
		{
			size_t const nodeCount{ createInfo.nodes.size() };
			for (size_t nodeIt{ }; nodeIt < nodeCount; ++nodeIt)
			{
				auto const &nodeInfo{ createInfo.nodes[nodeIt] };

				auto const pTransformMaterial{ renderEngine.createMaterial<TransformMaterial>() };
				pTransformMaterial->setTransform(nodeInfo.transform);
				transformMaterials.emplace_back(pTransformMaterial);

				for (uint32_t const drawInfoIdx : nodeInfo.drawInfoIndices)
				{
					auto const &drawInfo		{ createInfo.drawInfos[drawInfoIdx] };
					auto const &materialInfo	{ createInfo.materials[drawInfo.materialIdx] };

					auto const pRenderer	{ rendererFactory.getInstanceOf(materialInfo.rendererType) };
					auto const pMaterial	{ materials[drawInfo.materialIdx].get() };
					auto const pMesh		{ meshes[drawInfo.meshIdx].get() };
					auto const pDrawParam	{ drawParams[drawInfoIdx].get() };

					auto pRenderObject{ std::make_shared<Render::RenderObject>() };
					pRenderObject->setRenderer(pRenderer);
					pRenderObject->setMesh(pMesh);
					pRenderObject->setDrawParam(pDrawParam);

					auto &materialPack{ pRenderObject->getMaterialPackOf(0U) };
					materialPack.setMaterial(typeid(*pMaterial), pMaterial);
					materialPack.setMaterial(pTransformMaterial);

					renderObjects.emplace_back(std::move(pRenderObject));
				}
			}
		}
	}

	PhongMaterial *Model::__rcmd_createPhongMaterial(
		Render::Engine &renderEngine,
		std::vector<std::shared_ptr<Render::Texture>> const &textures,
		MaterialInfo const &materialInfo)
	{
		auto const pPhongMaterial{ renderEngine.createMaterial<PhongMaterial>() };

		pPhongMaterial->setAmbient(materialInfo.ambient);
		pPhongMaterial->setDiffuse(materialInfo.diffuse);
		pPhongMaterial->setSpecular(materialInfo.specular);
		pPhongMaterial->setEmissive(materialInfo.emissive);

		pPhongMaterial->setBlendOp(materialInfo.blendOp);
		pPhongMaterial->setOpacity(materialInfo.opacity);
		pPhongMaterial->setShininess(materialInfo.shininess);

		for (auto const &[texType, texInfos] : materialInfo.textureInfoMap)
		{
			for (size_t channel{ }; channel < texInfos.size(); ++channel)
			{
				auto const &texInfo	{ texInfos[channel] };
				auto const pTexture	{ textures[texInfo.index].get() };

				pPhongMaterial->setTexture(texType, channel, pTexture);
				pPhongMaterial->setTextureStrength(texType, channel, texInfo.strength);
				pPhongMaterial->setTextureBlendOp(texType, channel, texInfo.blendOp);
				pPhongMaterial->setTextureMapModeU(texType, channel, texInfo.mapModeU);
				pPhongMaterial->setTextureMapModeV(texType, channel, texInfo.mapModeV);
				pPhongMaterial->setTextureInverted(texType, channel, texInfo.inverted);
			}
		}

		return pPhongMaterial;
	}

	void Model::__rcmd_updateNodeTransforms(
		__RcmdResources const &resources,
		std::vector<glm::mat4> const &transforms)
	{
		auto const &transformMaterials{ resources.transformMaterials };

		size_t const nodeCount{ transforms.size() };
		for (size_t nodeIt{ }; nodeIt < nodeCount; ++nodeIt)
			transformMaterials[nodeIt]->setTransform(transforms[nodeIt]);
	}

	Model::__RcmdResources::__RcmdResources() noexcept
	{
		renderObjects.clear();
		transformMaterials.clear();

		textures.clear();
		materials.clear();
		meshes.clear();
		drawParams.clear();
	}
};