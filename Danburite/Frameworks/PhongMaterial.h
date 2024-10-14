#pragma once

#include "../Infra/GLM.h"
#include "../Render/Material.h"
#include "../Render/Texture.h"
#include "MaterialParams.h"
#include "Constants.h"

namespace Frx
{
	struct PhongMaterialData
	{
	public:
		alignas(16) glm::vec3 ambient	{ 0.f, 0.f, 0.f };
		alignas(16) glm::vec3 diffuse	{ 0.f, 0.f, 0.f };
		alignas(16) glm::vec3 specular	{ 0.f, 0.f, 0.f };
		alignas(16) glm::vec3 emissive	{ 0.f, 0.f, 0.f };

		AlphaBlendOp blendOp	{ AlphaBlendOp::DEFAULT };
		float opacity			{ 1.0f };
		float shininess			{ 0.0f };

		std::array<TextureParam, Constants::MAX_TEX_CHANNEL_COUNT> ambientTexs;
		std::array<TextureParam, Constants::MAX_TEX_CHANNEL_COUNT> diffuseTexs;
		std::array<TextureParam, Constants::MAX_TEX_CHANNEL_COUNT> specularTexs;
		std::array<TextureParam, Constants::MAX_TEX_CHANNEL_COUNT> emissiveTexs;
		std::array<TextureParam, Constants::MAX_TEX_CHANNEL_COUNT> normalTexs;
		std::array<TextureParam, Constants::MAX_TEX_CHANNEL_COUNT> opacityTexs;
		std::array<TextureParam, Constants::MAX_TEX_CHANNEL_COUNT> aoTexs;
	};

	class PhongMaterial : public Render::TypedMaterial<PhongMaterialData>
	{
	public:
		virtual ~PhongMaterial() noexcept override;

		void setAmbient(
			glm::vec3 const &color);

		void setDiffuse(
			glm::vec3 const &color);

		void setSpecular(
			glm::vec3 const &color);

		void setEmissive(
			glm::vec3 const &color);

		void setBlendOp(
			AlphaBlendOp blendOp);

		void setOpacity(
			float opacity);

		void setShininess(
			float shininess);

		void setTexture(
			TextureType type,
			size_t channel,
			Render::Texture const *pTexture);

		void setTextureStrength(
			TextureType type,
			size_t channel,
			float strength);

		void setTextureBlendOp(
			TextureType type,
			size_t channel,
			TextureBlendOp blendOp);

		void setTextureMapModeU(
			TextureType type,
			size_t channel,
			TextureMapMode mapMode);

		void setTextureMapModeV(
			TextureType type,
			size_t channel,
			TextureMapMode mapMode);

		void setTextureInverted(
			TextureType type,
			size_t channel,
			bool inverted);

	private:
		using __TexArr = std::array<Render::Texture const *, Constants::MAX_TEX_CHANNEL_COUNT>;
		std::unordered_map<TextureType, __TexArr> __texArrMap;

		[[nodiscard]]
		constexpr TextureParam *__getTexParamOf(
			TextureType type,
			size_t channel) noexcept;
	};

	constexpr TextureParam *PhongMaterial::__getTexParamOf(
		TextureType const type,
		size_t const channel) noexcept
	{
		TextureParam *texArr{ };

		switch (type)
		{
			case TextureType::AMBIENT:
				texArr = _getTypedData().ambientTexs.data();
				break;

			case TextureType::DIFFUSE:
				texArr = _getTypedData().diffuseTexs.data();
				break;

			case TextureType::SPECULAR:
				texArr = _getTypedData().specularTexs.data();
				break;

			case TextureType::EMISSIVE:
				texArr = _getTypedData().emissiveTexs.data();
				break;

			case TextureType::NORMALS:
				texArr = _getTypedData().normalTexs.data();
				break;

			case TextureType::OPACITY:
				texArr = _getTypedData().opacityTexs.data();
				break;

			case TextureType::AMBIENT_OCCLUSION:
				texArr = _getTypedData().aoTexs.data();
				break;
		}

		return (texArr ? (texArr + channel) : nullptr);
	}
}