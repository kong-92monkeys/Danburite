#pragma once

#include "../Infra/Event.h"
#include <future>
#include <optional>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace Infra
{
	class Executor : public Infra::Unique
	{
	public:
		using Job = std::function<void()>;

		Executor();
		virtual ~Executor() noexcept override;

		void waitIdle();

		[[nodiscard]]
		std::future<void> run(
			Job &&job);

		void silentRun(
			Job &&job);

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

		bool __running{ true };

		Infra::Event<Executor *> __idleEvent;

		void __loop();
	};

	constexpr Infra::EventView<Executor *> &Executor::exec_getIdleEvent() noexcept
	{
		return __idleEvent;
	}
}