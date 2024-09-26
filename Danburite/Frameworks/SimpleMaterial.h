#pragma once

#include "../Infra/GLM.h"
#include "../Render/Material.h"

namespace Frx
{
	struct SimpleMaterialData
	{
	public:
		glm::vec4 color;
	};

	class SimpleMaterial : public Render::TypedMaterial<SimpleMaterialData>
	{
	public:
		void setColor(
			glm::vec4 const &color) noexcept;
	};
}