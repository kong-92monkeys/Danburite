#include "PhongMaterial.h"

namespace Frx
{
	PhongMaterial::~PhongMaterial()
	{
		if (!__pAlbedoTexture)
			return;

		auto &imageRefManager{ _getImageReferenceManager() };
		imageRefManager.removeImage(&(__pAlbedoTexture->getImageView()));
	}

	void PhongMaterial::setShininess(
		float const shininess)
	{
		_getTypedData().shininess = shininess;
		_invokeUpdateEvent();
	}

	void PhongMaterial::setAlbedoTexture(
		Render::Texture const *const pTexture)
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
			_getTypedData().albedoTexId = static_cast<int>(imageRefManager.getIdOf(&imageView));
		}
		else
			_getTypedData().albedoTexId = -1;

		_invokeUpdateEvent();
	}
}