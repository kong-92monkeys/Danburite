#include "PhongMaterial.h"

namespace Frx
{
	void PhongMaterial::setAlbedoTexture(
		Render::Texture const *const pTexture) noexcept
	{
		if (__pAlbedoTexture == pTexture)
			return;

		auto const pPrevTexture{ __pAlbedoTexture };
		__pAlbedoTexture = pTexture;

		auto &imageRefManager{ _getImageReferenceManager() };

		if (pPrevTexture)
			imageRefManager.removeImage(&(pPrevTexture->getImageView()));

		if (__pAlbedoTexture)
		{
			auto &imageView{ __pAlbedoTexture->getImageView() };
			imageRefManager.addImage(&imageView);
			_getTypedData().imageId = static_cast<int>(imageRefManager.getIdOf(&imageView));
		}
		else
			_getTypedData().imageId = -1;

		_invokeUpdateEvent();
	}
}