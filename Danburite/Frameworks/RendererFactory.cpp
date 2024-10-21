#include "RendererFactory.h"
#include "PhongRenderer.h"
#include "GouraudRenderer.h"

namespace Frx
{
	RendererFactory::RendererFactory(
		Render::Engine &renderEngine) :
		__renderEngine{ renderEngine }
	{
		__renderers[RendererType::PHONG] =
			std::shared_ptr<Frx::PhongRenderer>{ __renderEngine.createRenderer<Frx::PhongRenderer>() };

		// FIXME
		__renderers[RendererType::GOURAUD] =
			std::shared_ptr<Frx::GouraudRenderer>{ __renderEngine.createRenderer<Frx::GouraudRenderer>() };
	}

	Render::Renderer const *RendererFactory::getInstanceOf(
		RendererType const type) const
	{
		return __renderers.at(type).get();
	}
}