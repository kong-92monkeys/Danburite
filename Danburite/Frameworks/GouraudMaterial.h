#pragma once

#include "../Infra/GLM.h"
#include "../Render/Material.h"
#include "../Render/Texture.h"
#include "MaterialParams.h"
#include "Constants.h"

namespace Frx
{
	struct GouraudMaterialData
	{
	public:
		alignas(16) glm::vec3 ambient	{ 0.f, 0.f, 0.f };
		alignas(16) glm::vec3 diffuse	{ 0.f, 0.f, 0.f };
		alignas(16) glm::vec3 emissive	{ 0.f, 0.f, 0.f };

		ColorBlendOp blendOp	{ ColorBlendOp::DEFAULT };
	};

	class GouraudMaterial : public Render::TypedMaterial<GouraudMaterialData>
	{
	public:
		virtual ~GouraudMaterial() noexcept override = default;

		void setAmbient(
			glm::vec3 const &color);

		void setDiffuse(
			glm::vec3 const &color);

		void setEmissive(
			glm::vec3 const &color);

		void setBlendOp(
			ColorBlendOp blendOp);
	};
}