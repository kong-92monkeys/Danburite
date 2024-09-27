#pragma once

#include "Unique.h"
#include <functional>
#include <future>

namespace Infra
{
	class ThreadPool : public Unique
	{
	public:
		using Job = std::function<void()>;

		virtual ~ThreadPool() noexcept override = default;

		virtual void waitIdle() = 0;

		[[nodiscard]]
		virtual std::future<void> run(
			Job &&job) = 0;

		virtual void silentRun(
			Job &&job) = 0;
	};
}