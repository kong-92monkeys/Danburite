#pragma once

#include "../Infra/ThreadPool.h"
#include "../Infra/ObjectHolder.h"
#include "../Infra/PointerHolder.h"

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

		template <typename $T>
		std::future<void> destroy(
			Infra::ObjectHolder<$T> &&pHolder);

		template <typename $T>
		std::future<void> destroy(
			Infra::PointerHolder<$T> &&pHolder);

		void silentRun(
			Job &&job);

		template <typename $T>
		void silentDestroy(
			Infra::ObjectHolder<$T> &&pHolder);

		template <typename $T>
		void silentDestroy(
			Infra::PointerHolder<$T> &&pHolder);

	private:
		Infra::ThreadPool __impl{ 1ULL };
	};

	template <typename $T>
	std::future<void> Executor::destroy(
		Infra::ObjectHolder<$T> &&pHolder)
	{
		return run([pHolder{ std::move(pHolder) }] { });
	}

	template <typename $T>
	std::future<void> Executor::destroy(
		Infra::PointerHolder<$T> &&pHolder)
	{
		return run([pHolder{ std::move(pHolder) }] { });
	}

	template <typename $T>
	void Executor::silentDestroy(
		Infra::ObjectHolder<$T> &&pHolder)
	{
		silentRun([pHolder{ std::move(pHolder) }] { });
	}

	template <typename $T>
	void Executor::silentDestroy(
		Infra::PointerHolder<$T> &&pHolder)
	{
		silentRun([pHolder{ std::move(pHolder) }] { });
	}
}