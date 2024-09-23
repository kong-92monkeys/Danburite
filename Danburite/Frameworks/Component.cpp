#include "Component.h"

namespace Frx
{
	Component::Component(
		bool const initialEnabledState) noexcept :
		__enabled{ initialEnabledState }
	{}

	void Component::init(
		Executor &rcmdExecutor,
		SceneObject &sceneObject,
		uint32_t const id)
	{
		__pRcmdExecutor	= &rcmdExecutor;
		__pSceneObject	= &sceneObject;
		__id			= id;

		_onInit();
	}

	void Component::setEnabled(
		bool const enabled)
	{
		if (__enabled == enabled)
			return;

		__enabled = enabled;
		__enabledChangeEvent.invoke(this, !__enabled, __enabled);
	}

	void Component::_onAttached()
	{}

	void Component::_onDetached()
	{}

	void Component::_onInit()
	{}
}