#pragma once

#include "Executor.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <memory>

namespace Infra
{
	class MultiThreadPool : public Executor
	{
	public:
		using Job = std::function<void()>;

		MultiThreadPool(
			size_t poolSize = std::thread::hardware_concurrency());

		virtual ~MultiThreadPool() noexcept override;

		virtual void waitIdle() override;

		[[nodiscard]]
		virtual std::future<void> run(
			Job &&job) override;

		virtual void silentRun(
			Job &&job) override;

		void waitIdle(
			size_t threadIndex);

		[[nodiscard]]
		std::future<void> run(
			size_t threadIndex,
			Job &&job);

		void silentRun(
			size_t threadIndex,
			Job &&job);

		[[nodiscard]]
		constexpr size_t getPoolSize() const noexcept;

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

		struct __SlotInfo
		{
		public:
			std::thread thread;
			std::mutex loopMutex;
			std::vector<__JobInfo> jobInfos;
			std::condition_variable loopCV;
		};

		std::vector<std::unique_ptr<__SlotInfo>> __slotInfos;
		std::vector<std::thread::id> __threadIds;

		bool __running{ true };

		size_t __randomSlotIndex{ };

		void __loop(const size_t threadIndex);
	};

	constexpr size_t MultiThreadPool::getPoolSize() const noexcept
	{
		return __slotInfos.size();
	}
}