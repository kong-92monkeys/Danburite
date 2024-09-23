#include "SceneObject.h"

namespace Frx
{
	SceneObject::SceneObject(
		Executor &rcmdExecutor) :
		__rcmdExecutor{ rcmdExecutor }
	{
		__pComponentEnabledChangeListener = Infra::EventListener<Component *, bool, bool>::bind(
			&SceneObject::__onComponentEnableChanged, this,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	}

	void SceneObject::setEnabled(
		bool const enabled)
	{
		if (__enabled == enabled)
			return;

		__enabled = enabled;

		if (__enabled)
			__enableObject();
		else
			__disableObject();
	}

	void SceneObject::removeComponent(
		uint32_t const id)
	{
		__componentIdAllocator.free(id);
		auto const pComponent{ std::move(__components.extract(id).mapped()) };

		if (pComponent->isEnabled())
		{
			pComponent->_onDetached();
			__enabledComponents.erase(pComponent.get());
		}

		__typeWiseComponents.at(typeid(*pComponent)).erase(pComponent.get());
	}

	void SceneObject::__enableObject()
	{
		for (auto const &[_, pComponent] : __components)
		{
			if (!(pComponent->isEnabled()))
				continue;

			__enabledComponents.emplace(pComponent.get());
			pComponent->_onAttached();
		}
	}

	void SceneObject::__disableObject()
	{
		for (auto const pComponent : __enabledComponents)
			pComponent->_onDetached();

		__enabledComponents.clear();
	}

	void SceneObject::__onComponentEnableChanged(
		Component *const pComponent,
		bool const prev,
		bool const cur)
	{
		if (!__enabled)
			return;

		if (cur)
		{
			__enabledComponents.emplace(pComponent);
			pComponent->_onAttached();
		}
		else
		{
			pComponent->_onDetached();
			__enabledComponents.erase(pComponent);
		}
	}
}