#pragma once

#include "Unique.h"
#include <functional>
#include <future>

namespace Infra
{
	class Executor : public Unique
	{
	public:
		using Job = std::function<void()>;

		virtual ~Executor() noexcept override = default;

		virtual void waitIdle() = 0;

		[[nodiscard]]
		virtual std::future<void> run(
			Job &&job) = 0;

		virtual void silentRun(
			Job &&job) = 0;
	};
}