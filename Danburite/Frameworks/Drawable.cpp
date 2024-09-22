#include "Drawable.h"

namespace Frx
{
	Drawable::Drawable(
		Executor &rcmdExecutor) :
		__rcmdExecutor{ rcmdExecutor }
	{
		__rcmdExecutor.silentRun([this]
		{
			__pRenderObject.create();
		});
	}

	Drawable::~Drawable() noexcept
	{
		__rcmdExecutor.silentDestroy(std::move(__pRenderObject));
	}
}