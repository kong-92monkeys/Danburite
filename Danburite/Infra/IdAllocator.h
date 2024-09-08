#pragma once

#include "Unique.h"
#include <queue>
#include <concepts>

namespace Infra
{
	template <std::integral $T>
	class IdAllocator : public Unique
	{
	public:
		[[nodiscard]]
		$T allocate() noexcept;

		void free(
			$T id) noexcept;

	private:
		$T __maxId{ };
		std::queue<$T> __standbyIds;
	};

	template <std::integral $T>
	$T IdAllocator<$T>::allocate() noexcept
	{
		$T retVal{ };

		if (__standbyIds.empty())
		{
			retVal = __maxId;
			++__maxId;
		}
		else
		{
			retVal = __standbyIds.front();
			__standbyIds.pop();
		}

		return retVal;
	}

	template <std::integral $T>
	void IdAllocator<$T>::free($T const id) noexcept
	{
		__standbyIds.emplace(id);
	}
}