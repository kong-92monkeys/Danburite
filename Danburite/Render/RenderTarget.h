#pragma once

#include <Windows.h>
#include "../Infra/GLM.h"
#include "../Vulkan/Queue.h"
#include "../Vulkan/Surface.h"
#include <memory>

namespace Render
{
	class RenderTarget : public Infra::Unique
	{
	public:
		RenderTarget(
			VK::Instance &instance,
			VK::PhysicalDevice &physicalDevice,
			VK::Device &device,
			VK::Queue &queue,
			HINSTANCE hinstance,
			HWND hwnd);

		virtual ~RenderTarget() noexcept override;

		void sync();

	private:
		VK::Instance &__instance;
		VK::PhysicalDevice &__physicalDevice;
		VK::Device &__device;
		VK::Queue &__que;

		std::unique_ptr<VK::Surface> __pSurface;
		
		void __createSurface(
			HINSTANCE hinstance,
			HWND hwnd);

		void __verifySurfaceSupport();
	};
}
