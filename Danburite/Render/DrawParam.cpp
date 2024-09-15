#include "DrawParam.h"

namespace Render
{
	DrawParamIndexed::DrawParamIndexed(
		uint32_t const indexCount,
		uint32_t const firstIndex,
		int32_t const vertexOffset) noexcept :
		__indexCount	{ indexCount },
		__firstIndex	{ firstIndex },
		__vertexOffset	{ vertexOffset }
	{}

	void DrawParamIndexed::draw(
		VK::CommandBuffer &cmdBuffer,
		uint32_t const instanceCount,
		uint32_t const firstInstance) const
	{
		cmdBuffer.vkCmdDrawIndexed(
			__indexCount, instanceCount,
			__firstIndex, __vertexOffset, firstInstance);
	}
}