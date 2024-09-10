#pragma once

#include "Unique.h"
#include "MathUtil.h"
#include <optional>
#include <map>

namespace Infra
{
	class RegionAllocator : public Unique
	{
	public:
		RegionAllocator(
			size_t size) noexcept;

		[[nodiscard]]
		std::optional<size_t> allocate(
			size_t size,
			size_t alignment) noexcept;

		void free(
			size_t offset) noexcept;

		[[nodiscard]]
		bool isEmpty() const noexcept;

	private:
		size_t const __size;
		std::map<size_t, size_t> __regions;
	};

	class Region : public Unique
	{
	public:
		Region(
			RegionAllocator &allocator,
			size_t size,
			size_t alignment);

		virtual ~Region() noexcept;

		[[nodiscard]]
		constexpr size_t getSize() const noexcept;

		[[nodiscard]]
		constexpr size_t getOffset() const noexcept;

	private:
		RegionAllocator &__regionAllocator;
		size_t const __size;
		size_t const __alignment;

		size_t __offset{ };

		void __create();
	};

	constexpr size_t Region::getSize() const noexcept
	{
		return __size;
	}

	constexpr size_t Region::getOffset() const noexcept
	{
		return __offset;
	}
}