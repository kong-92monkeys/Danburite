#pragma once

#include "Device.h"

namespace VK
{
	class Image : public Infra::Handle<VkImage>
	{
	public:
		Image(
			Device &device,
			VkImageCreateInfo const &createInfo);

		Image(
			VkImage handle,
			VkFormat format) noexcept;

		virtual ~Image() noexcept override;

		[[nodiscard]]
		constexpr VkMemoryDedicatedRequirements const &getMemoryDedicatedRequirements() const noexcept;

		[[nodiscard]]
		constexpr VkMemoryRequirements const &getMemoryRequirements() const noexcept;

	private:
		Device *__pDevice{ };

		VkFormat const __format;
		bool const __ownHandle;

		VkMemoryDedicatedRequirements __memDedicatedReq{ };
		VkMemoryRequirements2 __memReq2{ };

		void __resolveMemoryRequirements() noexcept;

		[[nodiscard]]
		static VkImage __create(
			Device &device,
			VkImageCreateInfo const &createInfo);
	};

	constexpr VkMemoryDedicatedRequirements const &Image::getMemoryDedicatedRequirements() const noexcept
	{
		return __memDedicatedReq;
	}

	constexpr VkMemoryRequirements const &Image::getMemoryRequirements() const noexcept
	{
		return __memReq2.memoryRequirements;
	}
}