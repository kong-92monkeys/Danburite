#include "Component.h"

namespace Frx
{
	void Component::init(
		Infra::ThreadPool &rcmdExecutor,
		Infra::Placeholder<std::shared_ptr<Render::RenderObject>> const &renderObject)
	{
		__pRcmdExecutor = &rcmdExecutor;
		__pRenderObject = &renderObject;

		_onInit();
	}

	void Component::_onInit()
	{}
}