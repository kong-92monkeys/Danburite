#pragma once

#include "../Infra/ThreadPool.h"

namespace Frx
{
	class Executor : public Infra::Unique
	{
	public:
		using Job = Infra::ThreadPool::Job;

		void waitIdle();

		[[nodiscard]]
		std::future<void> run(
			Job &&job);

		void silentRun(
			Job &&job);

	private:
		Infra::ThreadPool __impl{ 1ULL };
	};
}