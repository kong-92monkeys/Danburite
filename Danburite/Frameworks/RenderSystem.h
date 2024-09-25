#pragma once

#include "../Render/Engine.h"
#include "Placeholder.h"
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

		[[nodiscard]]
		Placeholder<Render::RenderTarget> createRenderTarget(
			HINSTANCE hinstance,
			HWND hwnd);

		[[nodiscard]]
		Placeholder<Render::Layer> createLayer();

		[[nodiscard]]
		Placeholder<Render::Mesh> createMesh();

		[[nodiscard]]
		Placeholder<Render::Texture> createTexture(
			Render::Texture::ImageCreateInfo const &imageCreateInfo,
			Render::Texture::ImageViewCreateInfo const &imageViewCreateInfo);

		template <std::derived_from<Render::Material> $Material, typename ...$Args>
		[[nodiscard]]
		Placeholder<$Material> createMaterial($Args &&...args);

		template <std::derived_from<Render::Renderer> $Renderer, typename ...$Args>
		[[nodiscard]]
		Placeholder<$Renderer> createRenderer($Args &&...args);

		[[nodiscard]]
		std::shared_ptr<SceneObject> createSceneObject();

		[[nodiscard]]
		constexpr Executor &getRcmdExecutor() noexcept;

	private:
		Executor __rcmdExecutor;
		std::unique_ptr<Render::Engine> __pEngine;

		void __createEngine(
			Dev::Context &context,
			VK::PhysicalDevice const &physicalDevice);
	};

	template <std::derived_from<Render::Material> $Material, typename ...$Args>
	Placeholder<$Material> RenderSystem::createMaterial($Args &&...args)
	{
		auto const pProm	{ new std::promise<$Material *> };
		auto fut			{ pProm->get_future() };

		__rcmdExecutor.silentRun([this, pProm, &args...] () mutable
		{
			pProm->set_value(__pEngine->createMaterial<$Material>(std::forward<$Args>(args)...));
			delete pProm;
		});

		return { __rcmdExecutor, std::move(fut) };
	}

	template <std::derived_from<Render::Renderer> $Renderer, typename ...$Args>
	Placeholder<$Renderer> RenderSystem::createRenderer($Args &&...args)
	{
		auto const pProm	{ new std::promise<$Renderer *> };
		auto fut			{ pProm->get_future() };

		__rcmdExecutor.silentRun([this, pProm, &args...] () mutable
		{
			pProm->set_value(__pEngine->createRenderer<$Renderer>(std::forward<$Args>(args)...));
			delete pProm;
		});

		return { __rcmdExecutor, std::move(fut) };
	}

	constexpr Executor &RenderSystem::getRcmdExecutor() noexcept
	{
		return __rcmdExecutor;
	}
}