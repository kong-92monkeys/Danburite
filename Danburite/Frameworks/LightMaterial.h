#pragma once

#include "../Infra/GLM.h"
#include "../Render/Material.h"

namespace Frx
{
	enum class LightType : uint32_t
	{
		DIRECTIONAL,
		POINT,
		SPOT
	};

	struct LightMaterialData
	{
	public:
		LightType type						{ LightType::DIRECTIONAL };
		float ambientFactor					{ 0.009f };
		float maxDistance					{ 32.0f };

		alignas(16) glm::vec3 color			{ 1.0f, 1.0f, 1.0f };

		alignas(16) glm::vec3 position		{ 0.0f, 0.0f, 0.0f };
		alignas(16) glm::vec3 direction		{ 0.0f, -1.0f, 0.0f };
		alignas(16) glm::vec3 attenuation	{ 1.0f, 0.22f, 0.20f };
	};

	class LightMaterial : public Render::TypedMaterial<LightMaterialData>
	{
	public:
		void setType(
			LightType type);

		void setAmbientFactor(
			float factor);

		void setMaxDistance(
			float distance);

		void setColor(
			glm::vec3 const &color);

		void setPosition(
			glm::vec3 const &position);

		void setDirection(
			glm::vec3 const &direction);

		void setAttenuation(
			glm::vec3 const &attenuation);
	};
}