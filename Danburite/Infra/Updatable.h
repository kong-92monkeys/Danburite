#pragma once

#include "Event.h"

namespace Infra
{
	template <typename $T>
	class Updatable
	{
	public:
		[[nodiscard]]
		constexpr EventView<$T const *> &getUpdateEvent() const noexcept;

	protected:
		void _invokeUpdateEvent() const noexcept;

	private:
		mutable Event<$T const *> __updateEvent;
	};

	template <typename $T>
	constexpr EventView<$T const *> &Updatable<$T>::getUpdateEvent() const noexcept
	{
		return __updateEvent;
	}

	template <typename $T>
	void Updatable<$T>::_invokeUpdateEvent() const noexcept
	{
		__updateEvent.invoke(this);
	}
}