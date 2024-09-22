#include "Material.h"

namespace Render
{
	Material::Material(
		bool const initialValidState) noexcept :
		__isValid{ initialValidState }
	{}

	void Material::init(
		ImageReferenceManager &imageReferenceManager)
	{
		__pImageReferenceManager = &imageReferenceManager;
	}

	void Material::_setValid(
		bool const valid)
	{
		if (__isValid == valid)
			return;

		__isValid = valid;
		__validChangeEvent.invoke(this, !valid, valid);
	}

	MaterialPack::MaterialPack()
	{
		__pMaterialValidChangeListener =
			Infra::EventListener<Material const *, bool, bool>::bind(
				&MaterialPack::__onMaterialValidChanged, this);
	}

	bool MaterialPack::hasValidMaterialOf(
		std::type_index const &type) const noexcept
	{
		auto const foundIt{ __materialMap.find(type) };
		if (foundIt == __materialMap.end())
			return false;

		auto const pMaterial{ foundIt->second.get() };
		if (!pMaterial)
			return false;

		return pMaterial->isValid();
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
		{
			pPrevMaterial->getValidChangeEvent() -= __pMaterialValidChangeListener;
			__materials.erase(pPrevMaterial.get());
		}

		holder = std::move(pMaterial);
		if (holder)
		{
			holder->getValidChangeEvent() += __pMaterialValidChangeListener;
			__materials.emplace(holder.get());
		}

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

	void MaterialPack::__onMaterialValidChanged()
	{
		__materialValidChangeEvent.invoke(this);
	}
}