#pragma once

#include "Unique.h"
#include <cstddef>
#include <memory>

namespace Infra
{
	template <typename $T>
	class PointerHolder
	{
	public:
		PointerHolder();
		~PointerHolder() noexcept;

		void emplace(
			$T *pointer);

		void destroy() noexcept;

		[[nodiscard]]
		$T *get() const noexcept;

		[[nodiscard]]
		$T *operator->() const noexcept;

		[[nodiscard]]
		$T &operator*() const noexcept;

		[[nodiscard]]
		operator bool() const noexcept;

	private:
		std::shared_ptr<$T *> __pImpl;
	};

	template <typename $T>
	PointerHolder<$T>::PointerHolder()
	{
		__pImpl = std::make_shared<$T *>(nullptr);
	}

	template <typename $T>
	PointerHolder<$T>::~PointerHolder() noexcept
	{
		delete get();
	}

	template <typename $T>
	void PointerHolder<$T>::emplace(
		$T *const pointer)
	{
		*__pImpl = pointer;
	}

	template <typename $T>
	void PointerHolder<$T>::destroy() noexcept
	{
		delete get();
	}

	template <typename $T>
	$T *PointerHolder<$T>::get() const noexcept
	{
		if (!__pImpl)
			return nullptr;

		return *__pImpl;
	}

	template <typename $T>
	$T *PointerHolder<$T>::operator->() const noexcept
	{
		return get();
	}

	template <typename $T>
	$T &PointerHolder<$T>::operator*() const noexcept
	{
		return *(get());
	}

	template <typename $T>
	PointerHolder<$T>::operator bool() const noexcept
	{
		return get();
	}
}