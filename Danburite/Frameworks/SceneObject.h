#pragma once

#include "../Infra/IdAllocator.h"
#include "Component.h"
#include <concepts>
#include <typeindex>
#include <unordered_set>

namespace Frx
{
	class SceneObject : public Infra::Unique
	{
	public:
		SceneObject(
			Executor &rcmdExecutor);

		[[nodiscard]]
		constexpr bool isEnabled() const noexcept;
		void setEnabled(
			bool enabled);

		template <std::derived_from<Component> $Component, typename ...$Args>
		$Component *addComponent(
			$Args &&...args);

		void removeComponent(
			uint32_t id);

		template <std::derived_from<Component> $Component>
		$Component *getComponentOf() const noexcept;

		template <std::derived_from<Component> $Component>
		std::vector<$Component *> getComponentsOf() const noexcept;

	private:
		Executor &__rcmdExecutor;

		bool __enabled{ true };

		Infra::IdAllocator<uint32_t> __componentIdAllocator;
		std::unordered_map<uint32_t, std::unique_ptr<Component>> __components;

		std::unordered_map<std::type_index, std::unordered_set<Component *>> __typeWiseComponents;
		std::unordered_set<Component *> __enabledComponents;

		Infra::EventListenerPtr<Component *, bool, bool> __pComponentEnabledChangeListener;

		void __enableObject();
		void __disableObject();

		void __onComponentEnableChanged(
			Component *pComponent,
			bool prev,
			bool cur);
	};

	constexpr bool SceneObject::isEnabled() const noexcept
	{
		return __enabled;
	}

	template <std::derived_from<Component> $Component, typename ...$Args>
	$Component *SceneObject::addComponent(
		$Args &&...args)
	{
		uint32_t const id{ __componentIdAllocator.allocate() };

		auto pComponent{ std::make_unique<$Component>(std::forward<$Args>(args)...) };
		pComponent->init(__rcmdExecutor, *this, id);
		pComponent->getEnabledChangeEvent() += __pComponentEnabledChangeListener;

		auto const pRetVal{ pComponent.get() };

		__components[id] = std::move(pComponent);
		__typeWiseComponents[typeid($Component)].emplace(pRetVal);

		if (pRetVal->isEnabled())
		{
			__enabledComponents.emplace(pRetVal);
			pRetVal->_onAttached();
		}

		return pRetVal;
	}

	template <std::derived_from<Component> $Component>
	$Component *SceneObject::getComponentOf() const noexcept
	{
		$Component *pRetVal{ };

		auto const foundIt{ __typeWiseComponents.find(typeid($Component)) };
		if (foundIt != __typeWiseComponents.end())
			pRetVal = *(foundIt->begin());

		return pRetVal;
	}

	template <std::derived_from<Component> $Component>
	std::vector<$Component *> SceneObject::getComponentsOf() const noexcept
	{
		std::vector<$Component *> retVal;

		auto const foundIt{ __typeWiseComponents.find(typeid($Component)) };
		if (foundIt != __typeWiseComponents.end())
		{
			for (auto const pComponent : *foundIt)
				retVal.emplace_back(static_cast<$Component *>(pComponent));
		}

		return retVal;
	}
}