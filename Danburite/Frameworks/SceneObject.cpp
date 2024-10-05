#include "SceneObject.h"

namespace Frx
{
	SceneObject::SceneObject() noexcept
	{
		__pTransformInvalidateListener =
			Infra::EventListener<Transform *>::bind(
				&SceneObject::__onTransformInvalidated, this);

		__pChildInvalidateListener =
			Infra::EventListener<SceneObject *>::bind(
				&SceneObject::__onChildInvalidated, this, std::placeholders::_1);

		__transform.getInvalidateEvent() += __pTransformInvalidateListener;
	}

	void SceneObject::addChild(
		std::shared_ptr<SceneObject> const &pChild)
	{
		if (!(__children.emplace(pChild).second))
			throw std::runtime_error{ "The display is already registered." };

		pChild->getInvalidateEvent() += __pChildInvalidateListener;
		__invalidatedChildren.emplace(pChild.get());

		_invalidate();
	}

	void SceneObject::removeChild(
		std::shared_ptr<SceneObject> const &pChild)
	{
		if (!(__children.erase(pChild)))
			throw std::runtime_error{ "The display is not registered yet." };

		pChild->getInvalidateEvent() -= __pChildInvalidateListener;
		__invalidatedChildren.erase(pChild.get());
	}

	void SceneObject::_onValidate()
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

	void SceneObject::_validate(
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

	void SceneObject::__onTransformInvalidated()
	{
		_invalidate();
	}

	void SceneObject::__onChildInvalidated(
		SceneObject *const pChild)
	{
		__invalidatedChildren.emplace(pChild);
		_invalidate();
	}
}