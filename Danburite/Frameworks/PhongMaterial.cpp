#include "PhongMaterial.h"

namespace Frx
{
	PhongMaterial::~PhongMaterial()
	{
		auto &imageRefManager{ _getImageReferenceManager() };

		for (auto const &[_, texArr] : __texArrMap)
		{
			for (auto const pTex : texArr)
			{
				if (pTex)
					imageRefManager.removeImage(&(pTex->getImageView()));
			}
		}
	}

	void PhongMaterial::setAmbient(
		glm::vec3 const &color)
	{
		_getTypedData().ambient = color;
		_invokeUpdateEvent();
	}

	void PhongMaterial::setDiffuse(
		glm::vec3 const &color)
	{
		_getTypedData().diffuse = color;
		_invokeUpdateEvent();
	}

	void PhongMaterial::setSpecular(
		glm::vec3 const &color)
	{
		_getTypedData().specular = color;
		_invokeUpdateEvent();
	}

	void PhongMaterial::setEmissive(
		glm::vec3 const &color)
	{
		_getTypedData().emissive = color;
		_invokeUpdateEvent();
	}

	void PhongMaterial::setBlendOp(
		AlphaBlendOp const blendOp)
	{
		_getTypedData().blendOp = blendOp;
		_invokeUpdateEvent();
	}

	void PhongMaterial::setOpacity(
		float const opacity)
	{
		_getTypedData().opacity = opacity;
		_invokeUpdateEvent();
	}

	void PhongMaterial::setShininess(
		float const shininess)
	{
		_getTypedData().shininess = shininess;
		_invokeUpdateEvent();
	}

	void PhongMaterial::setTexture(
		TextureType const type,
		size_t const channel,
		Render::Texture const *const pTexture)
	{
		auto &pPlaceholder{ __texArrMap[type][channel] };
		if (pPlaceholder == pTexture)
			return;

		auto const pPrevTexture{ pPlaceholder };
		pPlaceholder = pTexture;

		auto &imageRefManager	{ _getImageReferenceManager() };
		auto const pTexParam	{ __getTexParamOf(type, channel) };

		if (pPrevTexture)
			imageRefManager.removeImage(&(pPrevTexture->getImageView()));

		if (pPlaceholder)
		{
			auto &imageView{ pPlaceholder->getImageView() };
			imageRefManager.addImage(&imageView);

			if (pTexParam)
				pTexParam->id = static_cast<int>(imageRefManager.getIdOf(&imageView));
		}
		else
		{
			if (pTexParam)
				pTexParam->id = -1;
		}

		_invokeUpdateEvent();
	}

	void PhongMaterial::setTextureStrength(
		TextureType const type,
		size_t const channel,
		float const strength)
	{
		auto const pTexParam{ __getTexParamOf(type, channel) };
		if (!pTexParam)
			return;

		pTexParam->strength = strength;
		_invokeUpdateEvent();
	}

	void PhongMaterial::setTextureBlendOp(
		TextureType const type,
		size_t const channel,
		TextureBlendOp const blendOp)
	{
		auto const pTexParam{ __getTexParamOf(type, channel) };
		if (!pTexParam)
			return;

		pTexParam->blendOp = blendOp;
		_invokeUpdateEvent();
	}

	void PhongMaterial::setTextureMapModeU(
		TextureType const type,
		size_t const channel,
		TextureMapMode const mapMode)
	{
		auto const pTexParam{ __getTexParamOf(type, channel) };
		if (!pTexParam)
			return;

		pTexParam->mapModeU = mapMode;
		_invokeUpdateEvent();
	}

	void PhongMaterial::setTextureMapModeV(
		TextureType const type,
		size_t const channel,
		TextureMapMode const mapMode)
	{
		auto const pTexParam{ __getTexParamOf(type, channel) };
		if (!pTexParam)
			return;

		pTexParam->mapModeV = mapMode;
		_invokeUpdateEvent();
	}

	void PhongMaterial::setTextureInverted(
		TextureType const type,
		size_t const channel,
		bool const inverted)
	{
		auto const pTexParam{ __getTexParamOf(type, channel) };
		if (!pTexParam)
			return;

		pTexParam->inverted = inverted;
		_invokeUpdateEvent();
	}
}