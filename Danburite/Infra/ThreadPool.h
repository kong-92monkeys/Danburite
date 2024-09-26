#pragma once

#include "Unique.h"
#include <functional>
#include <thread>
#include <future>
#include <condition_variable>
#include <memory>

namespace Infra
{
	class ThreadPool : public Unique
	{
	public:
		using Job = std::function<void()>;

		ThreadPool(
			size_t poolSize = std::thread::hardware_concurrency());

		virtual ~ThreadPool() noexcept override;

		void waitIdle(
			size_t threadIndex);

		void waitIdle();

		[[nodiscard]]
		std::future<void> run(
			size_t threadIndex,
			Job &&job);

		[[nodiscard]]
		std::future<void> run(
			Job &&job);

		void silentRun(
			size_t threadIndex,
			Job &&job);

		void silentRun(
			Job &&job);

		[[nodiscard]]
		constexpr size_t getPoolSize() const noexcept;

	private:
		struct __JobInfo
		{
		public:
			Job job;
			std::optional<std::promise<void>> optPromise;

			void signal() noexcept;
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

	constexpr size_t ThreadPool::getPoolSize() const noexcept
	{
		return __slotInfos.size();
	}
}