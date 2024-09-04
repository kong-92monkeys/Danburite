#pragma once

#include "pch.h"
#include "../Infra/Logger.h"
#include <mutex>
#include <vector>
#include <string>

class ListBoxLoggerImpl : public Infra::Logger::Impl
{
public:
	explicit ListBoxLoggerImpl(
		CListBox &listBox) noexcept;

	virtual void log(
		std::string message) noexcept override;

	void flush();

private:
	CListBox &__listBox;

	std::mutex __mutex;
	std::vector<std::string> __logBuffer;
};