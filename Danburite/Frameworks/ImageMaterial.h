#pragma once

#include "../Render/Material.h"
#include "../Render/Texture.h"

namespace Frx
{
	struct ImageMaterialData
	{
	public:
		uint32_t imageId;
	};

	class ImageMaterial : public Render::TypedMaterial<ImageMaterialData>
	{
	public:
		ImageMaterial() noexcept;

		void setTexture(
			Render::Texture const *pTexture) noexcept;

	private:
		Render::Texture const *__pTexture{ };
	};
}