#pragma once

#include "Transform.h"
#include <unordered_set>

namespace Frx
{
	class SceneNode : public Infra::Stateful<SceneNode>
	{
	public:
		SceneNode() noexcept;
		SceneNode(
			glm::mat4 const &initialTransform);

		void addChild(
			SceneNode *pChild);

		void removeChild(
			SceneNode *pChild);

		[[nodiscard]]
		constexpr Transform &getTransform() noexcept;

		[[nodiscard]]
		constexpr Transform const &getTransform() const noexcept;

		[[nodiscard]]
		constexpr glm::mat4 const &getGlobalTransform() const noexcept;

	protected:
		virtual void _onValidate() override;

		void _validate(
			glm::mat4 const &parentGlobalTransform);

	private:
		Transform __transform;
		glm::mat4 __globalTransform{ 1.0f };

		std::unordered_set<SceneNode *> __children;
		std::unordered_set<SceneNode *> __invalidatedChildren;

		Infra::EventListenerPtr<Transform *> __pTransformInvalidateListener;
		Infra::EventListenerPtr<SceneNode *> __pChildInvalidateListener;

		void __onTransformInvalidated();

		void __onChildInvalidated(
			SceneNode *pChild);
	};

	constexpr Transform &SceneNode::getTransform() noexcept
	{
		return __transform;
	}

	constexpr Transform const &SceneNode::getTransform() const noexcept
	{
		return __transform;
	}

	constexpr glm::mat4 const &SceneNode::getGlobalTransform() const noexcept
	{
		return __globalTransform;
	}
}