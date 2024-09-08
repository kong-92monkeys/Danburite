#pragma once

#include "Unique.h"
#include <cstdint>
#include <cstddef>
#include <vector>

namespace Infra
{
	class Bitmap : public Unique
	{
	public:
		Bitmap(
			size_t width,
			size_t height,
			size_t channelCount) noexcept;
		
		Bitmap(
			size_t width,
			size_t height,
			size_t channelCount,
			size_t stride) noexcept;

		Bitmap(
			void const *pEncodedData,
			size_t size);
		
		Bitmap(
			void const *pEncodedData,
			size_t size,
			size_t stride);

		[[nodiscard]]
		constexpr size_t getWidth() const noexcept;

		[[nodiscard]]
		constexpr size_t getHeight() const noexcept;

		[[nodiscard]]
		constexpr size_t getChannelCount() const noexcept;

		[[nodiscard]]
		constexpr size_t getStride() const noexcept;

		[[nodiscard]]
		constexpr size_t getDataSize() const noexcept;

		[[nodiscard]]
		constexpr std::byte *getData() noexcept;

		[[nodiscard]]
		constexpr const std::byte *getData() const noexcept;

	private:
		size_t __width{ };
		size_t __height{ };
		size_t __channelCount{ };
		size_t __stride{ };

		std::vector<std::byte> __data;
	};

	constexpr size_t Bitmap::getWidth() const noexcept
	{
		return __width;
	}

	constexpr size_t Bitmap::getHeight() const noexcept
	{
		return __height;
	}

	constexpr size_t Bitmap::getChannelCount() const noexcept
	{
		return __channelCount;
	}

	constexpr size_t Bitmap::getStride() const noexcept
	{
		return __stride;
	}

	constexpr size_t Bitmap::getDataSize() const noexcept
	{
		return __data.size();
	}

	constexpr std::byte *Bitmap::getData() noexcept
	{
		return __data.data();
	}

	constexpr std::byte const *Bitmap::getData() const noexcept
	{
		return __data.data();
	}
}