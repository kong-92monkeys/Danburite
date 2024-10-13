#pragma once

#include "MaterialParams.h"
#include "../Render/Engine.h"

namespace Frx
{
	class RendererFactory : public Infra::Unique
	{
	public:
		RendererFactory(
			Render::Engine &renderEngine);

		[[nodiscard]]
		Render::Renderer const *getInstanceOf(
			RendererType const type) const;

	private:
		Render::Engine &__renderEngine;
		std::unordered_map<RendererType, std::shared_ptr<Render::Renderer>> __renderers;
	};
}