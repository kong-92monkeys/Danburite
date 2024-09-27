#pragma once

#include "ThreadPool.h"
#include <thread>
#include <mutex>
#include <optional>
#include <memory>

namespace Infra
{
	class SingleThreadPool : public ThreadPool
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
		struct __JobInfo
		{
		public:
			Job job;
			std::optional<std::promise<void>> optPromise;

			void signal() noexcept;
		};

		std::thread __thread;
		std::mutex __loopMutex;
		std::vector<__JobInfo> __jobInfos;
		std::condition_variable __loopCV;

		bool __running{ true };

		void __loop();
	};
}
