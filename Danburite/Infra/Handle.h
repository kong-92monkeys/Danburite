#pragma once

#include "Unique.h"

namespace Infra
{
	template <typename $H>
	class Handle : public Unique
	{
	public:
		Handle() = default;

		explicit Handle(
			$H handle) noexcept;

		[[nodiscard]]
		constexpr $H const &getHandle() const noexcept;

	protected:
		constexpr void _setHandle(
			$H handle) noexcept;

	private:
		$H __handle{ };
	};

	template <typename $H>
	Handle<$H>::Handle(
		$H const handle) noexcept :
		__handle	{ handle }
	{}

	template <typename $H>
	constexpr $H const &Handle<$H>::getHandle() const noexcept
	{
		return __handle;
	}

	template <typename $H>
	constexpr void Handle<$H>::_setHandle(
		$H const handle) noexcept
	{
		__handle = handle;
	}
}
