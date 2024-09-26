#include "RenderSystem.h"
#include "ImageMaterial.h"
#include "SimpleMaterial.h"
#include <new>

namespace Frx
{
	RenderSystem::RenderSystem(
		Dev::Context &context,
		VK::PhysicalDevice const &physicalDevice)
	{
		__rcmdExecutor.run([this, &context, &physicalDevice]
		{
			__pExecutorIdleListener =
				Infra::EventListener<Infra::Executor *>::bind(
					&RenderSystem::__rcmd_onIdle, this);

			__rcmdExecutor.exec_getIdleEvent() += __pExecutorIdleListener;
			__createEngine(context, physicalDevice);
		}).wait();
	}

	RenderSystem::~RenderSystem() noexcept
	{
		__rcmdExecutor.run([this]
		{
			__rcmdExecutor.exec_getIdleEvent() -= __pExecutorIdleListener;
			__getRenderEngine().~Engine();
		}).wait();
	}

	std::unique_ptr<Display> RenderSystem::createDisplay(
		HINSTANCE const hinstance,
		HWND const hwnd)
	{
		return std::make_unique<Display>(
			__rcmdExecutor, __getRenderEngine(), hinstance, hwnd);
	}

	void RenderSystem::__createEngine(
		Dev::Context &context,
		VK::PhysicalDevice const &physicalDevice)
	{
		Render::GlobalDescriptorManager::BindingInfo globalDescBindingInfo;
		globalDescBindingInfo.materialBufferLocations[typeid(SimpleMaterial)]	= 0U;
		globalDescBindingInfo.materialBufferLocations[typeid(ImageMaterial)]	= 1U;

		new (__enginePlaceholder.data()) Render::Engine{ context, physicalDevice, globalDescBindingInfo };
	}

	void RenderSystem::__rcmd_onIdle()
	{
		auto const curTime{ std::chrono::steady_clock::now() };
		if (__frameTime >= (curTime - __lastRenderTime))
			return;

		__lastRenderTime = curTime;
		__getRenderEngine().render();
	}

	Render::Engine &RenderSystem::__getRenderEngine() noexcept
	{
		return *(reinterpret_cast<Render::Engine *>(__enginePlaceholder.data()));
	}
}