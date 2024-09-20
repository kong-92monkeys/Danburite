#include "ImageMaterial.h"

namespace Frx
{
	void ImageMaterial::setTexture(
		std::shared_ptr<Render::Texture const> const &pTexture) noexcept
	{
		if (__pTexture == pTexture)
			return;

		__pTexture = pTexture;

		auto &imageRefManager	{ _getImageReferenceManager() };
		auto &imageView			{ __pTexture->getImageView() };

		imageRefManager.addImage(&imageView);
		_getTypedData().imageId = imageRefManager.getIdOf(&imageView);

		_invokeUpdateEvent();
	}

	bool ImageMaterial::isValid() const noexcept
	{
		return __pTexture.get();
	}
}