#pragma once

#include <initializer_list>
#include <vector>
#include <cstddef>

namespace Infra
{
	class GenericBuffer
	{
	public:
		void add(
			void const *pData,
			size_t size) noexcept;

		template <typename $T>
		void typedAdd(
			$T const *pData,
			size_t size) noexcept;

		template <typename $T>
		void typedAdd(
			$T const &data) noexcept;

		template <typename $T>
		void typedAdd(
			std::initializer_list<$T> const &data) noexcept;

		template <typename $T>
		void typedAdd(
			std::vector<$T> const &data) noexcept;

		void insert(
			size_t offset,
			void const *pData,
			size_t size) noexcept;

		template <typename $T>
		void typedInsert(
			size_t index,
			$T const *pData,
			size_t size) noexcept;

		template <typename $T>
		void typedInsert(
			size_t index,
			$T const &data) noexcept;

		template <typename $T>
		void typedInsert(
			size_t index,
			std::initializer_list<$T> const &data) noexcept;

		template <typename $T>
		void typedInsert(
			size_t index,
			std::vector<$T> const &data) noexcept;

		void set(
			size_t offset,
			void const *pData,
			size_t const size) noexcept;

		template <typename $T>
		void typedSet(
			size_t index,
			$T const *pData,
			size_t size) noexcept;

		template <typename $T>
		void typedSet(
			size_t index,
			$T const &data) noexcept;

		[[nodiscard]]
		constexpr std::byte *getData() noexcept;

		[[nodiscard]]
		constexpr std::byte const *getData() const noexcept;

		template <typename $T>
		[[nodiscard]]
		constexpr $T *getTypedData() noexcept;

		template <typename $T>
		[[nodiscard]]
		constexpr $T const *getTypedData() const noexcept;

		[[nodiscard]]
		constexpr size_t getSize() const noexcept;

		template <typename $T>
		[[nodiscard]]
		constexpr size_t getTypedSize() const noexcept;

		void clear() noexcept;
		
		void resize(
			size_t size) noexcept;

		template <typename $T>
		void typedResize(
			size_t size) noexcept;

	private:
		std::vector<std::byte> __buffer;
	};

	template <typename $T>
	void GenericBuffer::typedAdd(
		$T const *const pData,
		size_t const size) noexcept
	{
		add(pData, size * sizeof($T));
	}

	template <typename $T>
	void GenericBuffer::typedAdd(
		$T const &data) noexcept
	{
		typedAdd<$T>(&data, 1ULL);
	}

	template <typename $T>
	void GenericBuffer::typedAdd(
		std::initializer_list<$T> const &data) noexcept
	{
		typedAdd<$T>(data.begin(), data.size());
	}

	template <typename $T>
	void GenericBuffer::typedAdd(
		std::vector<$T> const &data) noexcept
	{
		typedAdd<$T>(data.data(), data.size());
	}

	template <typename $T>
	void GenericBuffer::typedInsert(
		size_t const index,
		$T const *const pData,
		size_t const size) noexcept
	{
		insert(index * sizeof($T), pData, size * sizeof($T));
	}

	template <typename $T>
	void GenericBuffer::typedInsert(
		size_t const index,
		$T const &data) noexcept
	{
		typedInsert<$T>(index, &data, 1ULL);
	}

	template <typename $T>
	void GenericBuffer::typedInsert(
		size_t const index,
		std::initializer_list<$T> const &data) noexcept
	{
		typedInsert<$T>(index, data.begin(), data.size());
	}

	template <typename $T>
	void GenericBuffer::typedInsert(
		size_t const index,
		std::vector<$T> const &data) noexcept
	{
		typedInsert<$T>(index, data.data(), data.size());
	}

	template <typename $T>
	void GenericBuffer::typedSet(
		size_t const index,
		$T const *const pData,
		size_t const size) noexcept
	{
		set(index * sizeof($T), pData, size * sizeof($T));
	}

	template <typename $T>
	void GenericBuffer::typedSet(
		size_t const index,
		$T const &data) noexcept
	{
		typedSet<$T>(index, &data, 1ULL);
	}

	constexpr std::byte *GenericBuffer::getData() noexcept
	{
		return __buffer.data();
	}

	constexpr std::byte const *GenericBuffer::getData() const noexcept
	{
		return __buffer.data();
	}

	template <typename $T>
	constexpr $T *GenericBuffer::getTypedData() noexcept
	{
		return reinterpret_cast<$T *>(getData());
	}

	template <typename $T>
	constexpr $T const *GenericBuffer::getTypedData() const noexcept
	{
		return reinterpret_cast<const $T *>(getData());
	}

	constexpr size_t GenericBuffer::getSize() const noexcept
	{
		return __buffer.size();
	}

	template <typename $T>
	constexpr size_t GenericBuffer::getTypedSize() const noexcept
	{
		return (getSize() / sizeof($T));
	}

	template <typename $T>
	void GenericBuffer::typedResize(const size_t size) noexcept
	{
		resize(size * sizeof($T));
	}
}