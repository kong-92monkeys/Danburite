#pragma once

#include "../Vulkan/Semaphore.h"
#include <vector>
#include <memory>

namespace Dev
{
	class SemaphoreCirculator : public Infra::Unique
	{
	public:
		SemaphoreCirculator(
			VK::Device &device,
			VkSemaphoreType type,
			size_t count);

		[[nodiscard]]
		constexpr VK::Semaphore &getNext() noexcept;

	private:
		std::vector<std::unique_ptr<VK::Semaphore>> __semaphores;
		size_t __index{ };
	};

	constexpr VK::Semaphore &SemaphoreCirculator::getNext() noexcept
	{
		auto &retVal{ *(__semaphores[__index]) };
		__index = ((__index + 1ULL) % __semaphores.size());
		return retVal;
	}
}