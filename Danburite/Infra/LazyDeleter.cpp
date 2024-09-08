#include "LazyDeleter.h"

namespace Infra
{
	LazyDeleter::LazyDeleter(
		size_t const queueSize) noexcept
	{
		__garbageQueue.resize(queueSize);
	}

	LazyDeleter::~LazyDeleter() noexcept
	{
		flush();
	}

	size_t LazyDeleter::getQueueSize() const noexcept
	{
		return __garbageQueue.size();
	}

	void LazyDeleter::setQueueSize(
		size_t const size)
	{
		size_t const oldSize{ getQueueSize() };
		if (size == oldSize)
			return;

		for (size_t iter{ size }; iter < oldSize; ++iter)
			__garbageQueue.pop_front();

		__garbageQueue.resize(size);
	}

	void LazyDeleter::advance()
	{
		auto holder{ std::move(__garbageQueue.front()) };
		holder.clear();

		__garbageQueue.pop_front();
		__garbageQueue.emplace_back(std::move(holder));
	}

	void LazyDeleter::flush()
	{
		for (auto &holder : __garbageQueue)
			holder.clear();
	}
}