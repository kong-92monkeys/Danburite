#include "Component.h"

namespace Frx
{
	void Component::init(
		Executor &rcmdExecutor,
		SceneObject &sceneObject)
	{
		__pRcmdExecutor		= &rcmdExecutor;
		__pSceneObject		= &sceneObject;

		_onInit();
	}

	void Component::_onInit()
	{}
}