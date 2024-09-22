#pragma once

#include "../Infra/ThreadPool.h"
#include <memory>

namespace Frx
{
	template <typename $T>
	class Managed : public Infra::Unique
	{
	public:
		template <typename ...$Args>
		Managed(
			Infra::ThreadPool &executor,
			$Args &&...args);

		virtual ~Managed() noexcept override;

		[[nodiscard]]
		constexpr $T *get() const noexcept;

		[[nodiscard]]
		constexpr $T *operator->() const noexcept;

		[[nodiscard]]
		constexpr $T &operator*() const noexcept;

	private:
		Infra::ThreadPool &__executor;
		$T *__pData{ };
	};

	template <typename $T>
	template <typename ...$Args>
	Managed<$T>::Managed(
		Infra::ThreadPool &executor,
		$Args &&...args) :
		__executor{ executor }
	{
		__pData = new $T{ std::forward<$Args>(args)... };
	}

	template <typename $T>
	Managed<$T>::~Managed() noexcept
	{
		__executor.silentRun([pData{ __pData }]
		{
			delete pData;
		});
	}

	template <typename $T>
	constexpr $T *Managed<$T>::get() const noexcept
	{
		return __pData;
	}

	template <typename $T>
	constexpr $T *Managed<$T>::operator->() const noexcept
	{
		return get();
	}

	template <typename $T>
	constexpr $T &Managed<$T>::operator*() const noexcept
	{
		return *(get());
	}
}