#include "Model.h"

namespace Frx
{
	Model::Model(
		CreateInfo &&createInfo)
	{
		for (auto const &node : createInfo.nodes)
		{
			__sceneNodes.emplace_back(new SceneNode{ node.transform });
		}

	}

	Model::~Model() noexcept
	{
		for (auto const pNode : __sceneNodes)
			delete pNode;
	}
}