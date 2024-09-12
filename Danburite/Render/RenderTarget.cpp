#include "RenderTarget.h"

namespace Render
{
	RenderTarget::RenderTarget(
		VK::Instance &instance,
		VK::Device &device,
		VK::Queue &queue,
		HINSTANCE const hinstance,
		HWND const hwnd) :
		__instance	{ instance },
		__device	{ device },
		__que		{ queue }
	{
		__createSurface(hinstance, hwnd);
	}

	RenderTarget::~RenderTarget() noexcept
	{
		__que.vkQueueWaitIdle();

		__pSurface = nullptr;
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
}