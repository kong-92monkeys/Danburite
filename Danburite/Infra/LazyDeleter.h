#pragma once

#include "Unique.h"
#include <deque>
#include <vector>
#include <any>

namespace Infra
{
	class LazyDeleter : public Unique
	{
	public:
		explicit LazyDeleter(
			size_t queueSize) noexcept;

		virtual ~LazyDeleter() noexcept override;

		[[nodiscard]]
		size_t getQueueSize() const noexcept;

		void setQueueSize(
			size_t size);

		template <typename $T>
		void reserve($T &&garbage) noexcept;

		void advance();
		void flush();

	private:
		std::deque<std::vector<std::any>> __garbageQueue;
	};

	template <typename $T>
	void LazyDeleter::reserve($T &&garbage) noexcept
	{
		__garbageQueue.back().emplace_back(std::forward<$T>(garbage));
	}
}