#pragma once

#include "../Infra/GenericBuffer.h"
#include "VertexAttribute.h"

namespace Frx::PrimitiveBuilder
{
	struct PrimitiveData
	{
	public:
		size_t vertexCount{ };
		size_t indexCount{ };

		Infra::GenericBuffer posBuffer;
		Infra::GenericBuffer uvBuffer;
		Infra::GenericBuffer normalBuffer;
		Infra::GenericBuffer colorBuffer;
		Infra::GenericBuffer tangentBuffer;

		Infra::GenericBuffer indexBuffer;
	};

	[[nodiscard]]
	PrimitiveData buildSquare(
		VertexAttribFlags attribFlags,
		float width,
		float height) noexcept;
}