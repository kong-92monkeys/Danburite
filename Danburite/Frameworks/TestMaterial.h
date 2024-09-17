#pragma once

#include "../Infra/GLM.h"
#include "../Render/Material.h"

namespace Frx
{
	struct TestMaterialData
	{
	public:
		glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
	};

	class TestMaterial : public Render::TypedMaterial<TestMaterialData>
	{
	public:
		void setColor(
			glm::vec4 const &color);
	};
}