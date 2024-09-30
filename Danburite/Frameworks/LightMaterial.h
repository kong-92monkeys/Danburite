#pragma once

#include "../Infra/GLM.h"
#include "../Render/Material.h"

namespace Frx
{
	struct LightMaterialData
	{
	public:
		uint32_t type{ };
		float ambientFactor{ 0.01f };

		alignas(16) glm::vec4 color{ };
	};

	class LightMaterial : public Render::TypedMaterial<LightMaterialData>
	{
	public:
		void setAmbientFactor(
			float factor);

		void setColor(
			glm::vec4 const &color);
	};
}