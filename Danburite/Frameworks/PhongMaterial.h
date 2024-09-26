#pragma once

#include "../Render/Material.h"
#include "../Render/Texture.h"

namespace Frx
{
	struct PhongMaterialData
	{
	public:
		int imageId{ -1 };
	};

	class PhongMaterial : public Render::TypedMaterial<PhongMaterialData>
	{
	public:
		void setAlbedoTexture(
			Render::Texture const *pTexture) noexcept;

	private:
		Render::Texture const *__pAlbedoTexture{ };
	};
}