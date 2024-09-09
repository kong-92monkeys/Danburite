#pragma once

#include "Unique.h"

namespace Infra
{
	template <typename $H>
	class Handle : public Unique
	{
	public:
		explicit Handle(
			$H handle) noexcept;

		[[nodiscard]]
		constexpr $H const &getHandle() noexcept;

	protected:
		[[nodiscard]]
		constexpr $H const &getHandle() const noexcept;

	private:
		$H const __handle;
	};

	template <typename $H>
	Handle<$H>::Handle(
		$H const handle) noexcept :
		__handle	{ handle }
	{}

	template <typename $H>
	constexpr $H const &Handle<$H>::getHandle() noexcept
	{
		return __handle;
	}

	template <typename $H>
	constexpr $H const &Handle<$H>::getHandle() const noexcept
	{
		return __handle;
	}
}
