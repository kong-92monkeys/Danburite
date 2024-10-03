#pragma once

#include "Event.h"
#include "Executor.h"
#include <thread>
#include <mutex>
#include <vector>

namespace Infra
{
	class Looper : public Executor
	{
	public:
		using Job = std::function<void()>;

		Looper();
		virtual ~Looper() noexcept override;

		virtual void waitIdle() override;

		[[nodiscard]]
		virtual std::future<void> run(
			Job &&job) override;

		virtual void silentRun(
			Job &&job) override;

		[[nodiscard]]
		constexpr EventView<Looper *> &in_getIdleEvent() noexcept;

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

		bool __running{ true };

		Event<Looper *> __idleEvent;

		void __loop();
	};

	constexpr EventView<Looper *> &Looper::in_getIdleEvent() noexcept
	{
		return __idleEvent;
	}
}