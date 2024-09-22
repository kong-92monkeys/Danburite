#include "Display.h"

namespace Frx
{
	Display::Display(
		Infra::ThreadPool &rcmdExecutor,
		Infra::Placeholder<Render::Engine> const &pEngine,
		HINSTANCE const hinstance,
		HWND const hwnd) :
		__rcmdExecutor	{ rcmdExecutor }
	{

	}

	Display::~Display() noexcept
	{

	}
}