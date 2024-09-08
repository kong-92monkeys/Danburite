#pragma once

#include <concepts>

namespace Infra::MathUtil
{
	template <std::integral $T>
	constexpr $T getGCDOf(
		$T const lhs,
		$T const rhs) noexcept
	{
		$T lIter{ lhs };
		$T rIter{ rhs };

		while (rIter)
		{
			$T const remainder{ lIter % rIter };
			lIter = rIter;
			rIter = remainder;
		}

		return lIter;
	}

	template <std::integral $T>
	constexpr $T getLCMOf(
		$T const lhs,
		$T const rhs) noexcept
	{
		return (rhs * (lhs / getGCDOf(lhs, rhs)));
	}

	template <std::integral $T>
	constexpr $T ceilAlign(
		$T const value,
		$T const alignment) noexcept
	{
		return (value + ((alignment - (value % alignment)) % alignment));
	}

	template <std::integral $T>
	constexpr $T floorAlign(
		$T const value,
		$T const alignment) noexcept
	{
		return (value - (value % alignment));
	}
}