#include "Executor.h"

namespace Frx
{
	void Executor::waitIdle()
	{
		__impl.waitIdle();
	}

	std::future<void> Executor::run(
		Job &&job)
	{
		return __impl.run(std::move(job));
	}

	void Executor::silentRun(
		Job &&job)
	{
		__impl.silentRun(std::move(job));
	}
}