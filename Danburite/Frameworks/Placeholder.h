#pragma once

#include "Executor.h"

namespace Frx
{
	template <typename $T>
	class Placeholder
	{
	public:
		Placeholder() = default;
		Placeholder(
			Executor &executor,
			std::future<$T *> &&fut);

		void create(
			Executor &executor,
			std::future<$T *> &&fut);

		void reset();

		[[nodiscard]]
		$T *get() const noexcept;

	private:
		std::shared_ptr<std::shared_future<$T *>> __pFut;
	};

	template <typename $T>
	Placeholder<$T>::Placeholder(
		Executor &executor,
		std::future<$T *> &&fut)
	{
		create(executor, std::move(fut));
	}

	template <typename $T>
	void Placeholder<$T>::create(
		Executor &executor,
		std::future<$T *> &&fut)
	{
		__pFut = std::shared_ptr<std::shared_future<$T *>>
		{
			new std::shared_future<$T *>{ std::move(fut) },
			[&executor] (std::shared_future<$T *> *const pFut)
			{
				executor.silentRun([pFut]
				{
					delete pFut->get();
					delete pFut;
				});
			}
		};
	}

	template <typename $T>
	void Placeholder<$T>::reset()
	{
		__pFut.reset();
	}

	template <typename $T>
	$T *Placeholder<$T>::get() const noexcept
	{
		return __pFut->get();
	}
}