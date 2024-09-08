#include "GenericBuffer.h"

namespace Infra
{
	void GenericBuffer::add(
		void const *const pData,
		size_t const size) noexcept
	{
		size_t const prevSize{ __buffer.size() };
		__buffer.resize(prevSize + size);

		std::memcpy(__buffer.data() + prevSize, pData, size);
	}

	void GenericBuffer::insert(
		size_t const offset,
		void const *const pData,
		size_t const size) noexcept
	{
		auto const iter{ __buffer.begin() + offset };
		auto const pSrc{ static_cast<std::byte const *>(pData) };
		__buffer.insert(iter, pSrc, pSrc + size);
	}

	void GenericBuffer::set(
		size_t const offset,
		void const *const pData,
		size_t const size) noexcept
	{
		std::memcpy(__buffer.data() + offset, pData, size);
	}

	void GenericBuffer::clear() noexcept
	{
		__buffer.clear();
	}

	void GenericBuffer::resize(size_t const size) noexcept
	{
		__buffer.resize(size);
	}
}