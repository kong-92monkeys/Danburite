#include "Material.h"

namespace Render
{
	uint32_t Material::getTextureSlotCount() const noexcept
	{
		return 0U;
	}

	void Material::_setTexture(
		uint32_t const slotIndex,
		Texture const *const pTexture) noexcept
	{
		auto &pPlaceholder{ __textureSlots[slotIndex] };
		if (pPlaceholder == pTexture)
			return;

		auto const pPrevTexture{ pPlaceholder };
		pPlaceholder = pTexture;

		__textureChangeEvent.invoke(this, slotIndex, pPrevTexture, pTexture);
	}

	bool MaterialPack::hasMaterialOf(
		std::type_index const &type) const noexcept
	{
		auto const foundIt{ __materialMap.find(type) };
		if (foundIt == __materialMap.end())
			return false;

		return foundIt->second.get();
	}

	void MaterialPack::setMaterial(
		std::type_index const &type,
		std::shared_ptr<Material const> pMaterial) noexcept
	{
		auto &holder{ __materialMap[type] };
		if (holder == pMaterial)
			return;

		auto pPrevMaterial{ std::move(holder) };
		if (pPrevMaterial)
			__materials.erase(pPrevMaterial.get());

		holder = std::move(pMaterial);
		if (holder)
			__materials.emplace(holder.get());

		__materialChangeEvent.invoke(this, type, pPrevMaterial.get(), holder.get());
	}

	std::unordered_set<const Material *>::const_iterator MaterialPack::begin() const noexcept
	{
		return __materials.begin();
	}

	std::unordered_set<const Material *>::const_iterator MaterialPack::end() const noexcept
	{
		return __materials.end();
	}
}