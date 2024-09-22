#pragma once

#include "../Infra/Placeholder.h"
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
		Infra::ThreadPool __rcmdExecutor{ 1ULL };

		Infra::Placeholder<Render::Engine> __engine;

		void __createEngine(
			Dev::Context &context,
			VK::PhysicalDevice const &physicalDevice);
	};
}
