#pragma once

#include "RenderSystem.h"

namespace Frx
{
	class Scene : public Infra::Unique
	{
	public:
		Scene(
			RenderSystem &renderSystem);

	private:
		RenderSystem &__renderSystem;
	};
}