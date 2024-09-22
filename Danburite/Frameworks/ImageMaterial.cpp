#include "ImageMaterial.h"

namespace Frx
{
	void ImageMaterial::setTexture(
		Render::Texture const *const pTexture) noexcept
	{
		if (__pTexture == pTexture)
			return;

		auto const pPrevTexture{ __pTexture };
		__pTexture = pTexture;

		auto &imageRefManager{ _getImageReferenceManager() };

		if (pPrevTexture)
			imageRefManager.removeImage(&(pPrevTexture->getImageView()));

		if (__pTexture)
		{
			auto &imageView{ __pTexture->getImageView() };
			imageRefManager.addImage(&imageView);
			_getTypedData().imageId = imageRefManager.getIdOf(&imageView);
		}
		
		_setValid(__pTexture);
		_invokeUpdateEvent();
	}
}