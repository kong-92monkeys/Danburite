#pragma once

#include "Transform.h"
#include <unordered_set>

namespace Frx
{
	class SceneObject : public Infra::Stateful<SceneObject>
	{
	public:
		SceneObject() noexcept;

		void addChild(
			std::shared_ptr<SceneObject> const &pChild);

		void removeChild(
			std::shared_ptr<SceneObject> const &pChild);

		[[nodiscard]]
		constexpr Transform &getTransform() noexcept;

		[[nodiscard]]
		constexpr Transform const &getTransform() const noexcept;

	protected:
		virtual void _onValidate() override;

		void _validate(
			glm::mat4 const &parentGlobalTransform);

	private:
		Transform __transform;
		glm::mat4 __globalTransform{ 1.0f };

		std::unordered_set<std::shared_ptr<SceneObject>> __children;
		std::unordered_set<SceneObject *> __invalidatedChildren;

		Infra::EventListenerPtr<Transform *> __pTransformInvalidateListener;
		Infra::EventListenerPtr<SceneObject *> __pChildInvalidateListener;

		void __onTransformInvalidated();

		void __onChildInvalidated(
			SceneObject *pChild);
	};

	constexpr Transform &SceneObject::getTransform() noexcept
	{
		return __transform;
	}

	constexpr Transform const &SceneObject::getTransform() const noexcept
	{
		return __transform;
	}
}