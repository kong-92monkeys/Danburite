#include "RendererFactory.h"
#include "PhongRenderer.h"

namespace Frx
{
	RendererFactory::RendererFactory(
		Render::Engine &renderEngine) :
		__renderEngine{ renderEngine }
	{
		__renderers[RendererType::PHONG] =
			std::shared_ptr<Frx::PhongRenderer>{ __renderEngine.createRenderer<Frx::PhongRenderer>() };

		// FIXME
		__renderers[RendererType::GOURAUD] = __renderers[RendererType::PHONG];
	}

	Render::Renderer const *RendererFactory::getInstanceOf(
		RendererType const type) const
	{
		return __renderers.at(type).get();
	}
}