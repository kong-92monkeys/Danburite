#pragma once

#include "Executor.h"
#include <thread>
#include <mutex>
#include <optional>

namespace Infra
{
	class SingleThreadPool : public Executor
	{
	public:
		SingleThreadPool();
		virtual ~SingleThreadPool() noexcept override;

		virtual void waitIdle() override;

		[[nodiscard]]
		virtual std::future<void> run(
			Job &&job) override;

		virtual void silentRun(
			Job &&job) override;

	private:
		class __JobInfo
		{
		public:
			__JobInfo(
				Job &&job,
				std::optional<std::promise<void>> optPromise) noexcept;

			void run();

		private:
			Job __job;
			std::optional<std::promise<void>> __optPromise;
		};

		std::thread __thread;
		std::mutex __loopMutex;
		std::vector<__JobInfo> __jobInfos;
		std::condition_variable __loopCV;

		bool __running{ true };

		void __loop();
	};
}
