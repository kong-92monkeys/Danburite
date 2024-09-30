#pragma once

#include "../Infra/GLM.h"
#include "../Render/Material.h"

namespace Frx
{
	struct TransformMaterialData
	{
	public:
		glm::mat4 transform{ 1.0f };
	};

	class TransformMaterial : public Render::TypedMaterial<TransformMaterialData>
	{
	public:
		void setTransform(
			glm::mat4 const &transform);
	};
}