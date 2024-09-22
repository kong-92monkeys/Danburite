#pragma once

#include <cstdint>
#include <array>

namespace Infra
{
	template <typename $T>
	class Placeholder
	{
	public:
		Placeholder();
		~Placeholder() noexcept;

		Placeholder(
			Placeholder const &src) = delete;

		constexpr Placeholder(
			Placeholder &&src) noexcept;

		template <typename ...$Args>
		void instantiate(
			$Args &&...args);

		void reset() noexcept;

		[[nodiscard]]
		constexpr $T *get() const noexcept;

		[[nodiscard]]
		constexpr $T *operator->() const noexcept;

		[[nodiscard]]
		constexpr $T &operator*() const noexcept;

		[[nodiscard]]
		constexpr operator bool() const noexcept;

		Placeholder &operator=(
			Placeholder const &src) = delete;

		constexpr Placeholder &operator=(
			Placeholder &&src) noexcept;

	private:
		std::byte *__pPlaceholder{ };
		bool __instantiated{ };

		void __destruct() noexcept;
	};

	template <typename $T>
	Placeholder<$T>::Placeholder()
	{
		__pPlaceholder = new std::byte[sizeof($T)];
	}

	template <typename $T>
	Placeholder<$T>::~Placeholder() noexcept
	{
		__destruct();
		delete[] __pPlaceholder;
	}

	template <typename $T>
	constexpr Placeholder<$T>::Placeholder(
		Placeholder &&src) noexcept
	{
		__pPlaceholder = src.__pPlaceholder;
		__instantiated = src.__instantiated;

		src.__pPlaceholder = new std::byte[sizeof($T)];
		src.__instantiated = false;
	}

	template <typename $T>
	template <typename ...$Args>
	void Placeholder<$T>::instantiate(
		$Args &&...args)
	{
		reset();

		new (__pPlaceholder) $T{ std::forward<$Args>(args)... };
		__instantiated = true;
	}

	template <typename $T>
	void Placeholder<$T>::reset() noexcept
	{
		__destruct();
		__instantiated = false;
	}

	template <typename $T>
	void Placeholder<$T>::__destruct() noexcept
	{
		if (!__instantiated)
			return;

		get()->~$T();
	}

	template <typename $T>
	constexpr $T *Placeholder<$T>::get() const noexcept
	{
		if (__instantiated)
			return nullptr;

		return reinterpret_cast<$T *>(__pPlaceholder);
	}

	template <typename $T>
	constexpr $T *Placeholder<$T>::operator->() const noexcept
	{
		return get();
	}

	template <typename $T>
	constexpr $T &Placeholder<$T>::operator*() const noexcept
	{
		return *(get());
	}

	template <typename $T>
	constexpr Placeholder<$T>::operator bool() const noexcept
	{
		return __instantiated;
	}

	template <typename $T>
	constexpr Placeholder<$T> &Placeholder<$T>::operator=(
		Placeholder &&src) noexcept
	{
		__destruct();

		__pPlaceholder = src.__pPlaceholder;
		__instantiated = src.__instantiated;

		src.__pPlaceholder = new std::byte[sizeof($T)];
		src.__instantiated = false;
	}
}