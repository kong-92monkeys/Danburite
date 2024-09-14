#include "DeferredDeleter.h"

namespace Infra
{
	DeferredDeleter::DeferredDeleter(
		size_t const queueSize) noexcept
	{
		__garbageQueue.resize(queueSize);
	}

	DeferredDeleter::~DeferredDeleter() noexcept
	{
		flush();
	}

	size_t DeferredDeleter::getQueueSize() const noexcept
	{
		return __garbageQueue.size();
	}

	void DeferredDeleter::setQueueSize(
		size_t const size)
	{
		size_t const oldSize{ getQueueSize() };
		if (size == oldSize)
			return;

		for (size_t iter{ size }; iter < oldSize; ++iter)
			__garbageQueue.pop_front();

		__garbageQueue.resize(size);
	}

	void DeferredDeleter::advance()
	{
		auto holder{ std::move(__garbageQueue.front()) };
		holder.clear();

		__garbageQueue.pop_front();
		__garbageQueue.emplace_back(std::move(holder));
	}

	void DeferredDeleter::flush()
	{
		for (auto &holder : __garbageQueue)
			holder.clear();
	}
}