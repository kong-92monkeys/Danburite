#pragma once

#include "../Render/Material.h"
#include "../Render/Texture.h"

namespace Frx
{
	struct PhongMaterialData
	{
	public:
		float shininess{ 32.0f };
		int albedoTexId{ -1 };
	};

	class PhongMaterial : public Render::TypedMaterial<PhongMaterialData>
	{
	public:
		virtual ~PhongMaterial() noexcept override;

		void setShininess(
			float shininess);

		void setAlbedoTexture(
			Render::Texture const *pTexture);

	private:
		Render::Texture const *__pAlbedoTexture{ };
	};
}