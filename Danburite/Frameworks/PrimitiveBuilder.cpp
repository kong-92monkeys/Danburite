#include "PrimitiveBuilder.h"

namespace Frx::PrimitiveBuilder
{
	PrimitiveData buildSquare(
		VertexAttribFlags const attribFlags,
		float const size,
		float const uvSize) noexcept
	{
		PrimitiveData retVal;

		float const halfSize{ 0.5f * size };

		if (hasFlagBit(attribFlags, VertexAttribFlagBits::POS))
		{
			auto &posBuffer{ retVal.posBuffer };
			posBuffer.typedAdd<glm::vec3>({ -halfSize, halfSize, 0.0f });
			posBuffer.typedAdd<glm::vec3>({ -halfSize, -halfSize, 0.0f });
			posBuffer.typedAdd<glm::vec3>({ halfSize, -halfSize, 0.0f });
			posBuffer.typedAdd<glm::vec3>({ halfSize, halfSize, 0.0f });
		}

		if (hasFlagBit(attribFlags, VertexAttribFlagBits::UV0))
		{
			auto &uvBuffer{ retVal.uvBuffer };
			uvBuffer.typedAdd<glm::vec2>({ 0.0f, 0.0f });
			uvBuffer.typedAdd<glm::vec2>({ 0.0f, uvSize });
			uvBuffer.typedAdd<glm::vec2>({ uvSize, uvSize });
			uvBuffer.typedAdd<glm::vec2>({ uvSize, 0.0f });
		}

		if (hasFlagBit(attribFlags, VertexAttribFlagBits::NORMAL))
		{
			auto &normalBuffer{ retVal.normalBuffer };
			normalBuffer.typedAdd<glm::vec3>({ 0.0f, 0.0f, 1.0f });
			normalBuffer.typedAdd<glm::vec3>({ 0.0f, 0.0f, 1.0f });
			normalBuffer.typedAdd<glm::vec3>({ 0.0f, 0.0f, 1.0f });
			normalBuffer.typedAdd<glm::vec3>({ 0.0f, 0.0f, 1.0f });
		}

		if (hasFlagBit(attribFlags, VertexAttribFlagBits::COLOR))
		{
			auto &colorBuffer{ retVal.colorBuffer };
			colorBuffer.typedAdd<glm::vec4>({ 1.0f, 1.0f, 1.0f, 1.0f });
			colorBuffer.typedAdd<glm::vec4>({ 1.0f, 1.0f, 1.0f, 1.0f });
			colorBuffer.typedAdd<glm::vec4>({ 1.0f, 1.0f, 1.0f, 1.0f });
			colorBuffer.typedAdd<glm::vec4>({ 1.0f, 1.0f, 1.0f, 1.0f });
		}

		auto &indexBuffer{ retVal.indexBuffer };
		indexBuffer.typedAdd<uint16_t>({ 0U, 1U, 2U, 0U, 2U, 3U });

		retVal.indexType	= VkIndexType::VK_INDEX_TYPE_UINT16;
		retVal.vertexCount	= 4U;
		retVal.indexCount	= 6U;

		return retVal;
	}

	PrimitiveData buildCube(
		VertexAttribFlags const attribFlags,
		float const size) noexcept
	{
		PrimitiveData retVal;

		float const halfSize{ 0.5f * size };

		if (hasFlagBit(attribFlags, VertexAttribFlagBits::POS))
		{
			auto &posBuffer{ retVal.posBuffer };

			// +X
			posBuffer.typedAdd<glm::vec3>({ halfSize, halfSize, halfSize });
			posBuffer.typedAdd<glm::vec3>({ halfSize, -halfSize, halfSize });
			posBuffer.typedAdd<glm::vec3>({ halfSize, -halfSize, -halfSize });
			posBuffer.typedAdd<glm::vec3>({ halfSize, halfSize, -halfSize });

			// -X
			posBuffer.typedAdd<glm::vec3>({ -halfSize, halfSize, -halfSize });
			posBuffer.typedAdd<glm::vec3>({ -halfSize, -halfSize, -halfSize });
			posBuffer.typedAdd<glm::vec3>({ -halfSize, -halfSize, halfSize });
			posBuffer.typedAdd<glm::vec3>({ -halfSize, halfSize, halfSize });

			// +Y
			posBuffer.typedAdd<glm::vec3>({ -halfSize, halfSize, -halfSize });
			posBuffer.typedAdd<glm::vec3>({ -halfSize, halfSize, halfSize });
			posBuffer.typedAdd<glm::vec3>({ halfSize, halfSize, halfSize });
			posBuffer.typedAdd<glm::vec3>({ halfSize, halfSize, -halfSize });

			// -Y
			posBuffer.typedAdd<glm::vec3>({ -halfSize, -halfSize, halfSize });
			posBuffer.typedAdd<glm::vec3>({ -halfSize, -halfSize, -halfSize });
			posBuffer.typedAdd<glm::vec3>({ halfSize, -halfSize, -halfSize });
			posBuffer.typedAdd<glm::vec3>({ halfSize, -halfSize, halfSize });

			// +Z
			posBuffer.typedAdd<glm::vec3>({ -halfSize, halfSize, halfSize });
			posBuffer.typedAdd<glm::vec3>({ -halfSize, -halfSize, halfSize });
			posBuffer.typedAdd<glm::vec3>({ halfSize, -halfSize, halfSize });
			posBuffer.typedAdd<glm::vec3>({ halfSize, halfSize, halfSize });

			// -Z
			posBuffer.typedAdd<glm::vec3>({ halfSize, halfSize, -halfSize });
			posBuffer.typedAdd<glm::vec3>({ halfSize, -halfSize, -halfSize });
			posBuffer.typedAdd<glm::vec3>({ -halfSize, -halfSize, -halfSize });
			posBuffer.typedAdd<glm::vec3>({ -halfSize, halfSize, -halfSize });
		}

		if (hasFlagBit(attribFlags, VertexAttribFlagBits::UV0))
		{
			auto &uvBuffer{ retVal.uvBuffer };

			for (size_t planeIter{ }; planeIter < 6ULL; ++planeIter)
			{
				uvBuffer.typedAdd<glm::vec2>({ 0.0f, 0.0f });
				uvBuffer.typedAdd<glm::vec2>({ 0.0f, 1.0f });
				uvBuffer.typedAdd<glm::vec2>({ 1.0f, 1.0f });
				uvBuffer.typedAdd<glm::vec2>({ 1.0f, 0.0f });
			}
		}

		if (hasFlagBit(attribFlags, VertexAttribFlagBits::NORMAL))
		{
			auto &normalBuffer{ retVal.normalBuffer };

			// +X
			normalBuffer.typedAdd<glm::vec3>({ 1.0f, 0.0f, 0.0f });
			normalBuffer.typedAdd<glm::vec3>({ 1.0f, 0.0f, 0.0f });
			normalBuffer.typedAdd<glm::vec3>({ 1.0f, 0.0f, 0.0f });
			normalBuffer.typedAdd<glm::vec3>({ 1.0f, 0.0f, 0.0f });

			// -X
			normalBuffer.typedAdd<glm::vec3>({ -1.0f, 0.0f, 0.0f });
			normalBuffer.typedAdd<glm::vec3>({ -1.0f, 0.0f, 0.0f });
			normalBuffer.typedAdd<glm::vec3>({ -1.0f, 0.0f, 0.0f });
			normalBuffer.typedAdd<glm::vec3>({ -1.0f, 0.0f, 0.0f });

			// +Y
			normalBuffer.typedAdd<glm::vec3>({ 0.0f, 1.0f, 0.0f });
			normalBuffer.typedAdd<glm::vec3>({ 0.0f, 1.0f, 0.0f });
			normalBuffer.typedAdd<glm::vec3>({ 0.0f, 1.0f, 0.0f });
			normalBuffer.typedAdd<glm::vec3>({ 0.0f, 1.0f, 0.0f });

			// -Y
			normalBuffer.typedAdd<glm::vec3>({ 0.0f, -1.0f, 0.0f });
			normalBuffer.typedAdd<glm::vec3>({ 0.0f, -1.0f, 0.0f });
			normalBuffer.typedAdd<glm::vec3>({ 0.0f, -1.0f, 0.0f });
			normalBuffer.typedAdd<glm::vec3>({ 0.0f, -1.0f, 0.0f });

			// +Z
			normalBuffer.typedAdd<glm::vec3>({ 0.0f, 0.0f, 1.0f });
			normalBuffer.typedAdd<glm::vec3>({ 0.0f, 0.0f, 1.0f });
			normalBuffer.typedAdd<glm::vec3>({ 0.0f, 0.0f, 1.0f });
			normalBuffer.typedAdd<glm::vec3>({ 0.0f, 0.0f, 1.0f });

			// -Z
			normalBuffer.typedAdd<glm::vec3>({ 0.0f, 0.0f, -1.0f });
			normalBuffer.typedAdd<glm::vec3>({ 0.0f, 0.0f, -1.0f });
			normalBuffer.typedAdd<glm::vec3>({ 0.0f, 0.0f, -1.0f });
			normalBuffer.typedAdd<glm::vec3>({ 0.0f, 0.0f, -1.0f });
		}

		if (hasFlagBit(attribFlags, VertexAttribFlagBits::COLOR))
		{
			auto &colorBuffer{ retVal.colorBuffer };

			for (uint32_t vtxIter{ }; vtxIter < 24U; ++vtxIter)
				colorBuffer.typedAdd<glm::vec4>({ 1.0f, 1.0f, 1.0f, 1.0f });
		}

		auto &indexBuffer{ retVal.indexBuffer };

		for (uint16_t planeIter{ }; planeIter < 6Ui16; ++planeIter)
		{
			auto const baseIdx{ static_cast<uint16_t>(planeIter * 4Ui16) };

			indexBuffer.typedAdd<uint16_t>({
				baseIdx,
				static_cast<uint16_t>(baseIdx + 1Ui16),
				static_cast<uint16_t>(baseIdx + 2Ui16),
				baseIdx,
				static_cast<uint16_t>(baseIdx + 2Ui16),
				static_cast<uint16_t>(baseIdx + 3Ui16)
			});
		}

		retVal.indexType	= VkIndexType::VK_INDEX_TYPE_UINT16;
		retVal.vertexCount	= 24U;
		retVal.indexCount	= 36U;

		return retVal;
	}
}