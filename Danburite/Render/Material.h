#pragma once

#include "../Infra/Updatable.h"
#include "ImageReferenceManager.h"
#include <concepts>
#include <typeindex>
#include <unordered_set>
#include <unordered_map>

namespace Render
{
	class Material : public Infra::Updatable<Material>
	{
	public:
		Material(
			bool initialValidState) noexcept;

		void init(
			ImageReferenceManager &imageReferenceManager);

		[[nodiscard]]
		constexpr bool isValid() const noexcept;

		[[nodiscard]]
		virtual const std::byte *getData() const noexcept = 0;

		[[nodiscard]]
		virtual size_t getSize() const noexcept = 0;

		[[nodiscard]]
		constexpr Infra::EventView<Material const *, bool, bool> &getValidChangeEvent() const noexcept;

	protected:
		void _setValid(
			bool valid);

		[[nodiscard]]
		constexpr ImageReferenceManager &_getImageReferenceManager() const noexcept;

	private:
		ImageReferenceManager *__pImageReferenceManager{ };
		bool __isValid;

		mutable Infra::Event<Material const *, bool, bool> __validChangeEvent;
	};

	template <typename $Data>
	class TypedMaterial : public Material
	{
	public:
		TypedMaterial(
			bool initialValidState = true) noexcept;

		[[nodiscard]]
		virtual const std::byte *getData() const noexcept override final;

		[[nodiscard]]
		virtual size_t getSize() const noexcept override final;

	protected:
		[[nodiscard]]
		constexpr $Data &_getTypedData() noexcept;

		[[nodiscard]]
		constexpr const $Data &_getTypedData() const noexcept;

	private:
		$Data __data{ };
	};

	class MaterialPack : public Infra::Unique
	{
	public:
		MaterialPack();
		virtual ~MaterialPack() noexcept override = default;

		template <std::derived_from<Material> $M>
		[[nodiscard]]
		bool hasValidMaterialOf() const noexcept;

		[[nodiscard]]
		bool hasValidMaterialOf(
			std::type_index const &type) const noexcept;

		template <std::derived_from<Material> $M>
		void setMaterial(
			std::shared_ptr<$M const> pMaterial) noexcept;

		void setMaterial(
			std::type_index const &type,
			std::shared_ptr<Material const> pMaterial) noexcept;

		[[nodiscard]]
		std::unordered_set<Material const *>::const_iterator begin() const noexcept;

		[[nodiscard]]
		std::unordered_set<Material const *>::const_iterator end() const noexcept;

		[[nodiscard]]
		constexpr Infra::EventView<MaterialPack const *, std::type_index, Material const *, Material const *> &
			getMaterialChangeEvent() const noexcept;

		[[nodiscard]]
		constexpr Infra::EventView<MaterialPack const *> &
			getMaterialValidChangeEvent() const noexcept;

	private:
		std::unordered_map<std::type_index, std::shared_ptr<Material const>> __materialMap;
		std::unordered_set<Material const *> __materials;

		mutable Infra::Event<MaterialPack const *, std::type_index, Material const *, Material const *> __materialChangeEvent;
		mutable Infra::Event<MaterialPack const *> __materialValidChangeEvent;

		Infra::EventListenerPtr<Material const *, bool, bool> __pMaterialValidChangeListener;

		void __onMaterialValidChanged();
	};

	constexpr bool Material::isValid() const noexcept
	{
		return __isValid;
	}

	constexpr Infra::EventView<Material const *, bool, bool> &Material::getValidChangeEvent() const noexcept
	{
		return __validChangeEvent;
	}

	constexpr ImageReferenceManager &Material::_getImageReferenceManager() const noexcept
	{
		return *__pImageReferenceManager;
	}

	template <typename $Data>
	TypedMaterial<$Data>::TypedMaterial(
		bool const initialValidState) noexcept :
		Material{ initialValidState }
	{}

	template <typename $Data>
	std::byte const *TypedMaterial<$Data>::getData() const noexcept
	{
		return reinterpret_cast<const std::byte *>(&__data);
	}

	template <typename $Data>
	size_t TypedMaterial<$Data>::getSize() const noexcept
	{
		return sizeof($Data);
	}

	template <typename $Data>
	constexpr $Data &TypedMaterial<$Data>::_getTypedData() noexcept
	{
		return __data;
	}

	template <typename $Data>
	constexpr const $Data &TypedMaterial<$Data>::_getTypedData() const noexcept
	{
		return __data;
	}

	template <std::derived_from<Material> $M>
	bool MaterialPack::hasValidMaterialOf() const noexcept
	{
		return hasValidMaterialOf(typeid($M));
	}

	template <std::derived_from<Material> $M>
	void MaterialPack::setMaterial(
		std::shared_ptr<$M const> pMaterial) noexcept
	{
		setMaterial(typeid(*pMaterial), pMaterial);
	}

	constexpr Infra::EventView<MaterialPack const *, std::type_index, Material const *, Material const *> &
		MaterialPack::getMaterialChangeEvent() const noexcept
	{
		return __materialChangeEvent;
	}

	constexpr Infra::EventView<MaterialPack const *> &
		MaterialPack::getMaterialValidChangeEvent() const noexcept
	{
		return __materialValidChangeEvent;
	}
}