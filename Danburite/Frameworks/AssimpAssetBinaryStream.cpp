#include "AssimpAssetBinaryStream.h"
#include "../System/Env.h"

namespace Frx
{
	AssimpAssetBinaryStream::AssimpAssetBinaryStream(
		std::string_view const &assetPath)
	{
		auto &assetManager{ Sys::Env::getInstance().getAssetManager() };
		__asset = assetManager.readBinary(assetPath);
	}

	size_t AssimpAssetBinaryStream::Read(
		void *const pvBuffer,
		size_t const pSize,
		size_t const pCount)
	{
		size_t const readSize{ std::min(pSize * pCount, FileSize() - __cursor) };
		std::memcpy(pvBuffer, __asset.data() + __cursor, readSize);
		__cursor += readSize;

		return readSize;
	}

	size_t AssimpAssetBinaryStream::Write(
		void const *const pvBuffer,
		size_t const pSize,
		size_t const pCount)
	{
		return 0ULL;
	}

	aiReturn AssimpAssetBinaryStream::Seek(
		size_t const pOffset,
		aiOrigin const pOrigin)
	{
		auto const fileSize{ FileSize() };

		switch (pOrigin)
		{
			case aiOrigin::aiOrigin_SET:
				if (fileSize < pOffset)
					return aiReturn::aiReturn_FAILURE;

				__cursor = pOffset;
				break;

			case aiOrigin::aiOrigin_CUR:
				if (fileSize < (__cursor + pOffset))
					return aiReturn::aiReturn_FAILURE;

				__cursor += pOffset;
				break;

			case aiOrigin::aiOrigin_END:
				if (fileSize < pOffset)
					return aiReturn::aiReturn_FAILURE;

				__cursor = (fileSize - pOffset);
				break;
		}

		return aiReturn::aiReturn_SUCCESS;
	}

	size_t AssimpAssetBinaryStream::Tell() const
	{
		return __cursor;
	}

	size_t AssimpAssetBinaryStream::FileSize() const
	{
		return __asset.size();
	}

	void AssimpAssetBinaryStream::Flush()
	{}
}