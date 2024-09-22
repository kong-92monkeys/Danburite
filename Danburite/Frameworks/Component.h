#pragma once

#include "../Infra/PointerHolder.h"
#include "SceneObject.h"

namespace Frx
{
	class Component : public Infra::Unique
	{
	public:
		void init(
			Executor &rcmdExecutor,
			SceneObject &sceneObject);

	protected:
		virtual void _onInit();

		[[nodiscard]]
		constexpr Executor &_getRcmdExecutor() const noexcept;

		[[nodiscard]]
		constexpr SceneObject &_getSceneObject() const noexcept;

	private:
		Executor *__pRcmdExecutor{ };
		SceneObject *__pSceneObject{ };
	};

	constexpr Executor &Component::_getRcmdExecutor() const noexcept
	{
		return *__pRcmdExecutor;
	}

	constexpr SceneObject &Component::_getSceneObject() const noexcept
	{
		return *__pSceneObject;
	}
}