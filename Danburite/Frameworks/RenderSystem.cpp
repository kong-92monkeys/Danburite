#include "RenderSystem.h"
#include "Constants.h"
#include "TransformMaterial.h"
#include "PhongMaterial.h"
#include "LightMaterial.h"
#include <new>

namespace Frx
{
	RenderSystem::RenderSystem(
		Dev::Context &context,
		VK::PhysicalDevice const &physicalDevice)
	{
		__rcmdExecutor.run([this, &context, &physicalDevice]
		{
			__createEngine(context, physicalDevice);
			__createRenderFactory();
		}).wait();
	}

	RenderSystem::~RenderSystem() noexcept
	{
		__rcmdExecutor.run([this]
		{
			__getRendererFactory().~RendererFactory();
			__getRenderEngine().~Engine();
		}).wait();
	}

	Display *RenderSystem::createDisplay(
		HINSTANCE const hinstance,
		HWND const hwnd,
		bool const useDepthBuffer,
		bool const useStencilBuffer)
	{
		return new Display
		{
			__rcmdExecutor, __getRenderEngine(),
			hinstance, hwnd, useDepthBuffer, useStencilBuffer
		};
	}

	void RenderSystem::__createEngine(
		Dev::Context &context,
		VK::PhysicalDevice const &physicalDevice)
	{
		// NOTE: Need to sync with Constants.glsl

		Render::GlobalDescriptorManager::BindingInfo globalDescBindingInfo;
		globalDescBindingInfo.materialBufferLocations[typeid(TransformMaterial)]	= Constants::TRANSFORM_MATERIAL_LOCATION;
		globalDescBindingInfo.materialBufferLocations[typeid(PhongMaterial)]		= Constants::PHONG_MATERIAL_LOCATION;
		globalDescBindingInfo.materialBufferLocations[typeid(LightMaterial)]		= Constants::LIGHT_MATERIAL_LOCATION;

		new (__enginePlaceholder.data()) Render::Engine{ context, physicalDevice, globalDescBindingInfo };
	}

	void RenderSystem::__createRenderFactory()
	{
		new (__rendererFactoryPlaceholder.data()) RendererFactory{ __getRenderEngine() };
	}

	Render::Engine &RenderSystem::__getRenderEngine() noexcept
	{
		return *(reinterpret_cast<Render::Engine *>(__enginePlaceholder.data()));
	}

	RendererFactory &RenderSystem::__getRendererFactory() noexcept
	{
		return *(reinterpret_cast<RendererFactory *>(__rendererFactoryPlaceholder.data()));
	}
}