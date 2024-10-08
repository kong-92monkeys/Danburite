#include "SceneNode.h"

namespace Frx
{
	SceneNode::SceneNode() noexcept
	{
		__pTransformInvalidateListener =
			Infra::EventListener<Transform *>::bind(
				&SceneNode::__onTransformInvalidated, this);

		__pChildInvalidateListener =
			Infra::EventListener<SceneNode *>::bind(
				&SceneNode::__onChildInvalidated, this, std::placeholders::_1);

		__transform.getInvalidateEvent() += __pTransformInvalidateListener;
	}

	SceneNode::SceneNode(
		glm::mat4 const &initialTransform) :
		__transform{ initialTransform }
	{
		__globalTransform = __transform.getMatrix();

		__pTransformInvalidateListener =
			Infra::EventListener<Transform *>::bind(
				&SceneNode::__onTransformInvalidated, this);

		__pChildInvalidateListener =
			Infra::EventListener<SceneNode *>::bind(
				&SceneNode::__onChildInvalidated, this, std::placeholders::_1);

		__transform.getInvalidateEvent() += __pTransformInvalidateListener;
	}

	void SceneNode::addChild(
		SceneNode *const pChild)
	{
		if (!(__children.emplace(pChild).second))
			throw std::runtime_error{ "The display is already registered." };

		pChild->getInvalidateEvent() += __pChildInvalidateListener;
		__invalidatedChildren.emplace(pChild);

		_invalidate();
	}

	void SceneNode::removeChild(
		SceneNode *const pChild)
	{
		if (!(__children.erase(pChild)))
			throw std::runtime_error{ "The display is not registered yet." };

		pChild->getInvalidateEvent() -= __pChildInvalidateListener;
		__invalidatedChildren.erase(pChild);
	}

	void SceneNode::_onValidate()
	{
		if (__transform.isInvalidated())
		{
			__transform.validate();
			__globalTransform = __transform.getMatrix();

			for (auto const &pChild : __children)
				pChild->_validate(__globalTransform);
		}
		else
		{
			for (auto const pChild : __invalidatedChildren)
				pChild->_validate(__globalTransform);
		}

		__invalidatedChildren.clear();
	}

	void SceneNode::_validate(
		glm::mat4 const &parentGlobalTransform)
	{
		if (__transform.isInvalidated())
		{
			__transform.validate();
			__globalTransform = (parentGlobalTransform * __transform.getMatrix());

			for (auto const &pChild : __children)
				pChild->_validate(__globalTransform);
		}
		else
		{
			__globalTransform = (parentGlobalTransform * __transform.getMatrix());

			for (auto const pChild : __invalidatedChildren)
				pChild->_validate(__globalTransform);
		}

		__invalidatedChildren.clear();
		_markAsValidated();
	}

	void SceneNode::__onTransformInvalidated()
	{
		_invalidate();
	}

	void SceneNode::__onChildInvalidated(
		SceneNode *const pChild)
	{
		__invalidatedChildren.emplace(pChild);
		_invalidate();
	}
}