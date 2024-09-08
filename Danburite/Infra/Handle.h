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
		constexpr const $H &getHandle() const noexcept;

	private:
		$H const __handle;
	};

	template <typename $H>
	Handle<$H>::Handle(
		$H const handle) noexcept :
		__handle	{ handle }
	{}

	template <typename $H>
	constexpr const $H &Handle<$H>::getHandle() const noexcept
	{
		return __handle;
	}
}
