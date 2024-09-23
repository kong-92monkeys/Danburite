#pragma once

#include "Display.h"
#include "Canvas.h"
#include "Model.h"
#include "Style.h"
#include "Theme.h"
#include "Drawable.h"
#include "SceneObject.h"

namespace Frx
{
	class RenderSystem : public Infra::Unique
	{
	public:
		RenderSystem(
			Dev::Context &context,
			VK::PhysicalDevice const &physicalDevice);

		virtual ~RenderSystem() noexcept override;

		// TODO: 카메라 정보 넘기는 방법 고민해보기

		[[nodiscard]]
		std::unique_ptr<Display> createDisplay(
			HINSTANCE hinstance,
			HWND hwnd);

		[[nodiscard]]
		std::shared_ptr<Canvas> createCanvas();

		[[nodiscard]]
		std::shared_ptr<Model> createModel();

		template <std::derived_from<Render::Material> $Material, typename ...$Args>
		[[nodiscard]]
		std::shared_ptr<Style<$Material>> createStyle($Args &&...args);

		template <std::derived_from<Render::Renderer> $Renderer, typename ...$Args>
		[[nodiscard]]
		std::shared_ptr<Theme<$Renderer>> createTheme($Args &&...args);

		[[nodiscard]]
		std::shared_ptr<Drawable> createDrawable();

		[[nodiscard]]
		std::shared_ptr<SceneObject> createSceneObject();

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
