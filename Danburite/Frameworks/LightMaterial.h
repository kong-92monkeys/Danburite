#pragma once

#include "../Infra/GLM.h"
#include "../Render/Material.h"

namespace Frx
{
	struct LightMaterialData
	{
	public:
		glm::vec4 color{ };
	};

	class LightMaterial : public Render::TypedMaterial<LightMaterialData>
	{
	public:
		void setColor(
			glm::vec4 const &color);
	};
}