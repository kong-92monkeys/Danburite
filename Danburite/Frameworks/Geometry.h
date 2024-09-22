#pragma once

#include "../Infra/Placeholder.h"
#include "../Render/Engine.h"

namespace Frx
{
	class Geometry : public Infra::Unique
	{
	public:
		Geometry(
			Infra::ThreadPool &rcmdExecutor,
			Infra::Placeholder<Render::Engine> const &engine);

		virtual ~Geometry() noexcept override;

	private:
		Infra::ThreadPool &__rcmdExecutor;
		Infra::Placeholder<std::shared_ptr<Render::RenderTarget>> __renderTarget;
	};
}
