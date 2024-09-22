#include "Component.h"

namespace Frx
{
	void Component::init(
		Executor &rcmdExecutor,
		Infra::PointerHolder<Render::RenderObject> const &pRenderObject)
	{
		__pRcmdExecutor = &rcmdExecutor;
		__pRenderObject = pRenderObject;

		_onInit();
	}

	void Component::_onInit()
	{}
}