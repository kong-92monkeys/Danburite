#pragma once

#include "Unique.h"
#include <string>
#include <memory>
#include <mutex>
#include <vector>

namespace Infra
{
	class Logger : public Unique
	{
	public:
		enum class Severity
		{
			FATAL,
			WARNING,
			INFO,
			VERBOSE
		};

		class Impl : public Unique
		{
		public:
			virtual void log(
				std::string message) noexcept = 0;
		};

		static void emplaceImpl(
			std::shared_ptr<Impl> pImpl) noexcept;

		static void log(
			Severity severity,
			std::string message) noexcept;

	private:
		std::mutex __mutex;

		std::vector<std::string> __logBuffer;
		std::shared_ptr<Impl> __pImpl;

		Logger() = default;

		void __emplaceImpl(
			std::shared_ptr<Impl> &&pImpl) noexcept;

		void __log(
			Severity severity,
			std::string const &message) noexcept;

		[[nodiscard]]
		static Logger &__getInstance() noexcept;

		[[nodiscard]]
		static std::string __makeLogMessage(
			Severity severity,
			std::string const &message) noexcept;

		[[nodiscard]]
		static std::string __getCurrentTimeStr() noexcept;

		[[nodiscard]]
		static constexpr char const *__getSeverityStrOf(
			Severity severity) noexcept;
	};
}