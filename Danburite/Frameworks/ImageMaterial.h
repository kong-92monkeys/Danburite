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
		void setTexture(
			std::shared_ptr<Render::Texture const> const &pTexture) noexcept;

		[[nodiscard]]
		virtual bool isValid() const noexcept override;

	private:
		std::shared_ptr<Render::Texture const> __pTexture;
	};
}