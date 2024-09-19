#pragma once

#include <unordered_map>
#include <memory>
#include <stdexcept>

namespace Infra
{
	template <typename $T>
	class WeakReferenceSet
	{
	public:
		using Container = std::unordered_map<$T *, std::weak_ptr<$T>>;

		class Iterator
		{
		public:
			Iterator(
				Container::iterator &&iter) noexcept;

			void operator++();

			[[nodiscard]]
			$T &operator*() const noexcept;

			[[nodiscard]]
			constexpr bool operator!=(
				Iterator const &another) const noexcept;

		private:
			Container::iterator __iter;
		};

		[[nodiscard]]
		bool contains(
			std::shared_ptr<$T> const &ptr) const noexcept;
		
		void emplace(
			std::shared_ptr<$T> const &ptr);

		void erase(
			std::shared_ptr<$T> const &ptr);

		[[nodiscard]]
		Iterator begin() noexcept;

		[[nodiscard]]
		Iterator end() noexcept;

	private:
		Container __references;
	};

	template <typename $T>
	bool WeakReferenceSet<$T>::contains(
		std::shared_ptr<$T> const &ptr) const noexcept
	{
		return __references.contains(ptr.get());
	}

	template <typename $T>
	void WeakReferenceSet<$T>::emplace(
		std::shared_ptr<$T> const &ptr)
	{
		if (!ptr)
			throw std::runtime_error{ "Cannot emplace a null reference." };

		__references.emplace(ptr.get(), ptr);
	}

	template <typename $T>
	void WeakReferenceSet<$T>::erase(
		std::shared_ptr<$T> const &ptr)
	{
		if (!ptr)
			throw std::runtime_error{ "Cannot erase a null reference." };

		__references.erase(ptr.get());
	}

	template <typename $T>
	WeakReferenceSet<$T>::Iterator WeakReferenceSet<$T>::begin() noexcept
	{
		for (auto it{ __references.begin() }; it != __references.end(); )
		{
			auto const &[ptr, weakPtr] { *it };

			if (weakPtr.expired())
				it = __references.erase(it);
			else
				++it;
		}

		return { __references.begin() };
	}

	template <typename $T>
	WeakReferenceSet<$T>::Iterator WeakReferenceSet<$T>::end() noexcept
	{
		return { __references.end() };
	}

	template <typename $T>
	WeakReferenceSet<$T>::Iterator::Iterator(
		Container::iterator &&iter) noexcept :
		__iter{ std::move(iter) }
	{}

	template <typename $T>
	void WeakReferenceSet<$T>::Iterator::operator++()
	{
		++__iter;
	}

	template <typename $T>
	$T &WeakReferenceSet<$T>::Iterator::operator*() const noexcept
	{
		return *(__iter->first);
	}

	template <typename $T>
	constexpr bool WeakReferenceSet<$T>::Iterator::operator!=(
		Iterator const &another) const noexcept
	{
		return (__iter != another.__iter);
	}
}