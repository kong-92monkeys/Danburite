#pragma once

#include "Event.h"

namespace Infra
{
	template <typename $T>
	class Stateful : public Unique
	{
	public:
		void validate();

		[[nodiscard]]
		constexpr bool isInvalidated() const noexcept;

		[[nodiscard]]
		constexpr EventView<$T *> &getInvalidateEvent() noexcept;

	protected:
		void _invalidate() noexcept;
		virtual void _onValidate();

	private:
		bool __invalidated{ };

		Event<$T *> __invalidateEvent;
	};

	template <typename $T>
	void Stateful<$T>::validate()
	{
		if (!__invalidated)
			return;

		_onValidate();
		__invalidated = false;
	}

	template <typename $T>
	constexpr bool Stateful<$T>::isInvalidated() const noexcept
	{
		return __invalidated;
	}

	template <typename $T>
	constexpr EventView<$T *> &Stateful<$T>::getInvalidateEvent() noexcept
	{
		return __invalidateEvent;
	}

	template <typename $T>
	void Stateful<$T>::_invalidate() noexcept
	{
		__invalidated = true;
		__invalidateEvent.invoke(static_cast<$T *>(this));
	}

	template <typename $T>
	void Stateful<$T>::_onValidate() {}
}