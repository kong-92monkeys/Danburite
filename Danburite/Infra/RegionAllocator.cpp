#include "RegionAllocator.h"
#include <stdexcept>

namespace Infra
{
	RegionAllocator::RegionAllocator(
		size_t const size) noexcept :
		__size	{ size }
	{}

	std::optional<size_t> RegionAllocator::allocate(
		size_t const size,
		size_t const alignment) noexcept
	{
		std::optional<size_t> retVal;

		if (__regions.empty() && (__size >= size))
			retVal = 0ULL;
		else
		{
			for (auto regionIt{ __regions.begin() }; regionIt != __regions.end(); )
			{
				auto nextIt{ regionIt };
				++nextIt;

				auto const [itOffset, itSize] { *regionIt };

				size_t const from	{ MathUtil::ceilAlign(itOffset + itSize, alignment) };
				size_t const to		{ (nextIt == __regions.end()) ? __size : nextIt->first };

				if ((to - from) >= size)
				{
					retVal = from;
					break;
				}

				regionIt = nextIt;
			}
		}

		if (retVal.has_value())
			__regions.emplace(retVal.value(), size);

		return retVal;
	}

	void RegionAllocator::free(
		size_t const offset) noexcept
	{
		__regions.erase(offset);
	}

	bool RegionAllocator::isEmpty() const noexcept
	{
		return __regions.empty();
	}

	Region::Region(
		RegionAllocator &allocator,
		size_t const size,
		size_t const alignment) :
		__allocator		{ allocator },
		__size			{ size },
		__alignment		{ alignment }
	{
		__create();
	}

	Region::~Region() noexcept
	{
		__allocator.free(__offset);
	}

	void Region::__create()
	{
		auto const offset{ __allocator.allocate(__size, __alignment) };
		if (offset.has_value())
		{
			__offset = offset.value();
			return;
		}

		throw std::bad_alloc{ };
	}
}