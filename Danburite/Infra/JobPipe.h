#pragma once

#include "Executor.h"
#include <mutex>
#include <optional>

namespace Infra
{
	class JobPipe : public Executor
	{
	public:
		class JobInfo
		{
		public:
			JobInfo(
				Job &&job,
				std::optional<std::promise<void>> optPromise) noexcept;

			void run();

		private:
			Job __job;
			std::optional<std::promise<void>> __optPromise;
		};

		JobPipe() = default;

		virtual void waitIdle() override;

		[[nodiscard]]
		virtual std::future<void> run(
			Job &&job) override;

		virtual void silentRun(
			Job &&job) override;

		[[nodiscard]]
		void receive(
			std::vector<JobInfo> &&jobs);

	private:
		std::mutex __mutex;
		std::vector<JobInfo> __jobInfos;
	};
}