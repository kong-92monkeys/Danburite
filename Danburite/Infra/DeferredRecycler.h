#pragma once

#include "Unique.h"
#include "DeferredDeleter.h"
#include <memory>
#include <deque>
#include <functional>

namespace Infra
{
	template <typename $T>
	class DeferredRecycler : public Unique
	{
	public:
		explicit DeferredRecycler(
			DeferredDeleter &deleter) noexcept;

		[[nodiscard]]
		std::shared_ptr<$T> retrieve() noexcept;

		[[nodiscard]]
		std::shared_ptr<$T> retrieveWhere(
			std::function<bool($T const &)> &&test) noexcept;

		void recycle(
			std::shared_ptr<$T> &&pResource) noexcept;

		void standby(
			std::shared_ptr<$T> &&pResource) noexcept;

	private:
		using __ResourceContainer = std::deque<std::shared_ptr<$T>>;

		class __ResourceHolder : public Unique
		{
		public:
			__ResourceHolder(
				std::weak_ptr<__ResourceContainer> const &pContainer,
				std::shared_ptr<$T> &&resource) noexcept;

			virtual ~__ResourceHolder() noexcept override;

		private:
			std::weak_ptr<__ResourceContainer> const __pContainer;
			std::shared_ptr<$T> __pResource;
		};

		DeferredDeleter &__deleter;
		std::shared_ptr<__ResourceContainer> __pResourceContainer;
	};

	template <typename $T>
	DeferredRecycler<$T>::DeferredRecycler(
		DeferredDeleter &deleter) noexcept :
		__deleter	{ deleter }
	{
		__pResourceContainer = std::make_shared<std::deque<std::shared_ptr<$T>>>();
	}

	template <typename $T>
	std::shared_ptr<$T> DeferredRecycler<$T>::retrieve() noexcept
	{
		if (__pResourceContainer->empty())
			return nullptr;

		auto pFront{ std::move(__pResourceContainer->front()) };
		__pResourceContainer->pop_front();

		return pFront;
	}

	template <typename $T>
	std::shared_ptr<$T> DeferredRecycler<$T>::retrieveWhere(
		std::function<bool($T const &)> &&test) noexcept
	{
		if (__pResourceContainer->empty())
			return nullptr;

		std::shared_ptr<$T> pRetVal;

		for (auto iter{ __pResourceContainer->begin() }; iter != __pResourceContainer->end(); ++iter)
		{
			if (!(test(*(*iter))))
				continue;

			pRetVal = std::move(*iter);
			__pResourceContainer->erase(iter);
			break;
		}

		return pRetVal;
	}

	template <typename $T>
	void DeferredRecycler<$T>::recycle(
		std::shared_ptr<$T> &&pResource) noexcept
	{
		__deleter.reserve(std::make_shared<__ResourceHolder>(__pResourceContainer, std::move(pResource)));
	}

	template <typename $T>
	void DeferredRecycler<$T>::standby(
		std::shared_ptr<$T> &&pResource) noexcept
	{
		__pResourceContainer->emplace_back(std::move(pResource));
	}

	template <typename $T>
	DeferredRecycler<$T>::__ResourceHolder::__ResourceHolder(
		std::weak_ptr<__ResourceContainer> const &pContainer,
		std::shared_ptr<$T> &&pResource) noexcept :
		__pContainer	{ pContainer },
		__pResource		{ std::move(pResource) }
	{}

	template <typename $T>
	DeferredRecycler<$T>::__ResourceHolder::~__ResourceHolder() noexcept
	{
		auto const pContainer{ __pContainer.lock() };
		if (!pContainer)
			return;

		pContainer->emplace_back(std::move(__pResource));
	}
}