#pragma once

#include "Unique.h"
#include <cstddef>
#include <memory>

namespace Infra
{
	template <typename $T>
	class ObjectHolder
	{
	public:
		ObjectHolder();

		template <typename ...$Args>
		void create(
			$Args &&...args);

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
		std::shared_ptr<std::unique_ptr<$T>> __pImpl;
	};

	template <typename $T>
	ObjectHolder<$T>::ObjectHolder()
	{
		__pImpl = std::make_shared<std::unique_ptr<$T>>();
	}

	template <typename $T>
	template <typename ...$Args>
	void ObjectHolder<$T>::create(
		$Args &&...args)
	{
		*__pImpl = std::make_unique<$T>(std::forward<$Args>(args)...);
	}

	template <typename $T>
	void ObjectHolder<$T>::destroy() noexcept
	{
		*__pImpl = nullptr;
	}

	template <typename $T>
	$T *ObjectHolder<$T>::get() const noexcept
	{
		if (!__pImpl)
			return nullptr;

		return __pImpl->get();
	}

	template <typename $T>
	$T *ObjectHolder<$T>::operator->() const noexcept
	{
		return get();
	}

	template <typename $T>
	$T &ObjectHolder<$T>::operator*() const noexcept
	{
		return *(get());
	}

	template <typename $T>
	ObjectHolder<$T>::operator bool() const noexcept
	{
		return get();
	}
}