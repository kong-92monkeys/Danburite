#pragma once

#include "../Vulkan/Vulkan.h"
#include "../Infra/GenericBuffer.h"
#include "VertexAttribute.h"

namespace Frx::PrimitiveBuilder
{
	struct PrimitiveData
	{
	public:
		VkIndexType indexType{ };

		uint32_t vertexCount{ };
		uint32_t indexCount{ };

		Infra::GenericBuffer posBuffer;
		Infra::GenericBuffer uvBuffer;
		Infra::GenericBuffer normalBuffer;

		Infra::GenericBuffer indexBuffer;
	};

	[[nodiscard]]
	PrimitiveData buildSquare(
		float size = 1.0f,
		float uvSize = 1.0f) noexcept;

	[[nodiscard]]
	PrimitiveData buildCube(
		float size = 1.0f,
		float uvSize = 1.0f) noexcept;
}