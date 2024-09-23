#pragma once

#include "../Infra/Event.h"
#include "Executor.h"

namespace Frx
{
	class SceneObject;

	class Component : public Infra::Unique
	{
	public:
		Component(
			bool initialEnabledState = true) noexcept;

		void init(
			Executor &rcmdExecutor,
			SceneObject &sceneObject,
			uint32_t id);

		[[nodiscard]]
		constexpr bool isEnabled() const noexcept;
		void setEnabled(
			bool enabled);

		[[nodiscard]]
		constexpr uint32_t getId() const noexcept;

		[[nodiscard]]
		constexpr Infra::EventView<Component *, bool, bool> &
			getEnabledChangeEvent() noexcept;

		virtual void _onAttached();
		virtual void _onDetached();

	protected:
		virtual void _onInit();

		[[nodiscard]]
		constexpr Executor &_getRcmdExecutor() const noexcept;

		[[nodiscard]]
		constexpr SceneObject &_getSceneObject() const noexcept;

	private:
		Executor *__pRcmdExecutor{ };
		SceneObject *__pSceneObject{ };
		uint32_t __id{ };

		bool __enabled;

		Infra::Event<Component *, bool, bool> __enabledChangeEvent;
	};

	constexpr bool Component::isEnabled() const noexcept
	{
		return __enabled;
	}

	constexpr uint32_t Component::getId() const noexcept
	{
		return __id;
	}

	constexpr Infra::EventView<Component *, bool, bool> &
		Component::getEnabledChangeEvent() noexcept
	{
		return __enabledChangeEvent;
	}

	constexpr Executor &Component::_getRcmdExecutor() const noexcept
	{
		return *__pRcmdExecutor;
	}

	constexpr SceneObject &Component::_getSceneObject() const noexcept
	{
		return *__pSceneObject;
	}
}