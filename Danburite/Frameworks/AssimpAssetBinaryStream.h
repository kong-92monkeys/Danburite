#pragma once

#include <assimp/IOStream.hpp>
#include <vector>
#include <cstddef>

namespace Frx
{
	class AssimpAssetBinaryStream : public Assimp::IOStream
	{
	public:
		friend class AssimpAssetIOSystem;

		virtual ~AssimpAssetBinaryStream() noexcept override = default;

        virtual size_t Read(
            void *pvBuffer,
            size_t pSize,
            size_t pCount) override;

        virtual size_t Write(
            void const *pvBuffer,
            size_t pSize,
            size_t pCount) override;

        virtual aiReturn Seek(
            size_t pOffset,
            aiOrigin pOrigin) override;

        [[nodiscard]]
        virtual size_t Tell() const override;

        [[nodiscard]]
        virtual size_t FileSize() const override;

        virtual void Flush() override;

	protected:
		AssimpAssetBinaryStream(
			std::string_view const &assetPath);

	private:
        size_t __cursor{ };
		std::vector<std::byte> __asset;
	};
}