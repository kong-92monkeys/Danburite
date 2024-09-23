#include "Canvas.h"

namespace Frx
{
	Canvas::Canvas(
		Executor &rcmdExecutor,
		Infra::ObjectHolder<Render::Engine> const &pEngine) :
		__rcmdExecutor	{ rcmdExecutor }
	{
		__rcmdExecutor.silentRun([this, &pEngine]
		{
			__pLayer.emplace(pEngine->createLayer());
		});
	}

	Canvas::~Canvas() noexcept
	{
		__rcmdExecutor.silentDestroy(std::move(__pLayer));
	}
}