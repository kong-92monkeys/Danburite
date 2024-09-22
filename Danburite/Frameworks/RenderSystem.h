#pragma once

#include "Executor.h"
#include "../Render/Engine.h"

namespace Frx
{
	class RenderSystem : public Infra::Unique
	{
	public:
		RenderSystem(
			Dev::Context &context,
			VK::PhysicalDevice const &physicalDevice);

		virtual ~RenderSystem() noexcept override;

	private:
		Executor __rcmdExecutor;
		Infra::ObjectHolder<Render::Engine> __pEngine;

		void __createEngine(
			Dev::Context &context,
			VK::PhysicalDevice const &physicalDevice);
	};
}
