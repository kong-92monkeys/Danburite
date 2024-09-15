#pragma once

#include "../Infra/AssetManager.h"

namespace Sys
{
	class Env : public Infra::Unique
	{
	public:
		virtual ~Env() noexcept = default;

		[[nodiscard]]
		constexpr Infra::AssetManager &getAssetManager() noexcept;

		[[nodiscard]]
		static Env &getInstance();

	private:
		Env() = default;

		Infra::AssetManager __assetManager;
	};

	constexpr Infra::AssetManager &Env::getAssetManager() noexcept
	{
		return __assetManager;
	}
}