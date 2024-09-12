#pragma once

#include "../Vulkan/Fence.h"
#include <vector>
#include <memory>

namespace Dev
{
	class FenceCirculator : public Infra::Unique
	{
	public:
		FenceCirculator(
			VK::Device &device,
			size_t count);

		[[nodiscard]]
		constexpr VK::Fence &getNext() noexcept;

	private:
		std::vector<std::unique_ptr<VK::Fence>> __fences;
		size_t __index{ };
	};

	constexpr VK::Fence &FenceCirculator::getNext() noexcept
	{
		auto &retVal{ *(__fences[__index]) };
		__index = ((__index + 1ULL) % __fences.size());
		return retVal;
	}
}