#include "RenderTarget.h"
#include <stdexcept>

namespace Render
{
	RenderTarget::RenderTarget(
		VK::Instance &instance,
		VK::PhysicalDevice &physicalDevice,
		VK::Device &device,
		VK::Queue &queue,
		HINSTANCE const hinstance,
		HWND const hwnd) :
		__instance			{ instance },
		__physicalDevice	{ physicalDevice },
		__device			{ device },
		__que				{ queue }
	{
		__createSurface(hinstance, hwnd);
		sync();
	}

	RenderTarget::~RenderTarget() noexcept
	{
		__que.vkQueueWaitIdle();

		__pSurface = nullptr;
	}

	void RenderTarget::sync()
	{
		__verifySurfaceSupport();
	}

	void RenderTarget::__createSurface(
		HINSTANCE const hinstance,
		HWND const hwnd)
	{
		const VkWin32SurfaceCreateInfoKHR createInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR },
			.hinstance		{ hinstance },
			.hwnd			{ hwnd }
		};

		__pSurface = std::make_unique<VK::Surface>(__instance, createInfo);
	}

	void RenderTarget::__verifySurfaceSupport()
	{
		VkBool32 supported{ };
		__physicalDevice.vkGetPhysicalDeviceSurfaceSupportKHR(
			__que.getFamilyIndex(), __pSurface->getHandle(), &supported);

		if (!supported)
			throw std::runtime_error{ "The device doesn't support current surface." };
	}
}