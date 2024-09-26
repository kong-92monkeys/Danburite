#pragma once

#include "../Infra/Event.h"
#include <future>
#include <optional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

namespace Infra
{
	class Executor : public Infra::Unique
	{
	public:
		using Job = std::function<void()>;
		using Duration = std::chrono::steady_clock::duration;

		Executor();
		virtual ~Executor() noexcept override;

		void waitIdle();

		[[nodiscard]]
		std::future<void> run(
			Job &&job);

		void silentRun(
			Job &&job);

		constexpr void setLoopInterval(
			Duration const &interval);

		[[nodiscard]]
		constexpr Infra::EventView<Executor *> &exec_getIdleEvent() noexcept;

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

		Duration __loopInterval;

		bool __running{ true };

		Infra::Event<Executor *> __idleEvent;

		void __loop();
	};

	constexpr void Executor::setLoopInterval(
		Duration const &interval)
	{
		__loopInterval = interval;
	}

	constexpr Infra::EventView<Executor *> &Executor::exec_getIdleEvent() noexcept
	{
		return __idleEvent;
	}
}