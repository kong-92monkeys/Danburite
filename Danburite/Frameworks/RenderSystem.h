#pragma once

#include "Drawable.h"
#include "Model.h"
#include "Style.h"
#include "Theme.h"

namespace Frx
{
	class RenderSystem : public Infra::Unique
	{
	public:
		RenderSystem(
			Dev::Context &context,
			VK::PhysicalDevice const &physicalDevice);

		virtual ~RenderSystem() noexcept override;

		[[nodiscard]]
		std::shared_ptr<Drawable> createDrawable();

		// TODO: draw param 대응 이름 찾기

		[[nodiscard]]
		std::shared_ptr<Model> createModel();

		template <std::derived_from<Render::Material> $Material, typename ...$Args>
		[[nodiscard]]
		std::shared_ptr<Style<$Material>> createStyle($Args &&...args);

		template <std::derived_from<Render::Renderer> $Renderer, typename ...$Args>
		[[nodiscard]]
		std::shared_ptr<Theme<$Renderer>> createTheme($Args &&...args);

	private:
		Executor __rcmdExecutor;
		Infra::ObjectHolder<Render::Engine> __pEngine;

		void __createEngine(
			Dev::Context &context,
			VK::PhysicalDevice const &physicalDevice);
	};

	template <std::derived_from<Render::Material> $Material, typename ...$Args>
	std::shared_ptr<Style<$Material>> RenderSystem::createStyle($Args &&...args)
	{
		return std::make_shared<Style<$Material>>(
			__rcmdExecutor, __pEngine, std::forward<$Args>(args)...);
	}

	template <std::derived_from<Render::Renderer> $Renderer, typename ...$Args>
	std::shared_ptr<Theme<$Renderer>> RenderSystem::createTheme($Args &&...args)
	{
		return std::make_shared<Theme<$Renderer>>(
			__rcmdExecutor, __pEngine, std::forward<$Args>(args)...);
	}
}
