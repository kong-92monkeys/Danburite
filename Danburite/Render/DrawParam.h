#pragma once

#include "../Vulkan/CommandBuffer.h"

namespace Render
{
	class DrawParam
	{
	public:
		virtual void draw(
			VK::CommandBuffer &cmdBuffer,
			uint32_t instanceCount,
			uint32_t firstInstance) const = 0;
	};

	class DrawParamIndexed : public DrawParam
	{
	public:
		DrawParamIndexed(
			uint32_t indexCount,
			uint32_t firstIndex,
			int32_t vertexOffset) noexcept;

		virtual void draw(
			VK::CommandBuffer &cmdBuffer,
			uint32_t instanceCount,
			uint32_t firstInstance) const override;

	private:
		uint32_t const __indexCount;
		uint32_t const __firstIndex;
		int32_t const __vertexOffset;
	};
}