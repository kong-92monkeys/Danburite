#pragma once

namespace Infra
{
	class Unique
	{
	public:
		Unique() = default;
		Unique(Unique const &) = delete;
		Unique(Unique &&) = delete;

		virtual ~Unique() noexcept = default;

		Unique &operator=(Unique const &) = delete;
		Unique &operator=(Unique &&) = delete;
	};
}