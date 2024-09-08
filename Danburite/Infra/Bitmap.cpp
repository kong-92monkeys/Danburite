#include "Bitmap.h"
#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace Infra
{
	Bitmap::Bitmap(
		size_t const width,
		size_t const height,
		size_t const channelCount) noexcept :
		Bitmap{ width, height, channelCount, channelCount }
	{}

	Bitmap::Bitmap(
		size_t const width,
		size_t const height,
		size_t const channelCount,
		size_t const stride) noexcept :
		__width			{ width },
		__height		{ height },
		__channelCount	{ channelCount },
		__stride		{ stride }
	{
		__data.resize(width * height * stride);
	}

	Bitmap::Bitmap(void const *const pEncodedData, size_t const size) :
		Bitmap{ pEncodedData, size, 0ULL }
	{}

	Bitmap::Bitmap(void const *const pEncodedData, size_t const size, size_t const stride)
	{
		int width{ };
		int height{ };
		int channelCount{ };

		auto const pData
		{
			stbi_load_from_memory(
				static_cast<stbi_uc const *>(pEncodedData), static_cast<int>(size),
				&width, &height, &channelCount, static_cast<int>(stride))
		};

		if (!pData)
			throw std::runtime_error{ "Cannot decode the given data." };

		__width = static_cast<size_t>(width);
		__height = static_cast<size_t>(height);
		__channelCount = static_cast<size_t>(channelCount);
		__stride = stride;

		size_t const memSize{ __width * __height * __stride };
		__data.resize(memSize);

		std::memcpy(__data.data(), pData, memSize);
		stbi_image_free(pData);
	}
}