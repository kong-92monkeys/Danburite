#pragma once

#include "../Infra/Updatable.h"
#include <concepts>
#include <typeindex>
#include <unordered_set>
#include <unordered_map>

namespace Render
{
	class Material : public Infra::Updatable<Material>
	{
	public:
		[[nodiscard]]
		virtual const std::byte *getData() const noexcept = 0;

		[[nodiscard]]
		virtual size_t getSize() const noexcept = 0;
	};

	template <typename $Data>
	class TypedMaterial : public Material
	{
	public:
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
		MaterialPack() = default;
		virtual ~MaterialPack() noexcept override = default;

		template <std::derived_from<Material> $M>
		[[nodiscard]]
		bool hasMaterialOf() const noexcept;

		[[nodiscard]]
		bool hasMaterialOf(
			std::type_index const &type) const noexcept;

		template <std::derived_from<Material> $M>
		void setMaterial(
			std::shared_ptr<Material const> pMaterial) noexcept;

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

	private:
		std::unordered_map<std::type_index, std::shared_ptr<Material const>> __materialMap;
		std::unordered_set<Material const *> __materials;

		mutable Infra::Event<MaterialPack const *, std::type_index, Material const *, Material const *> __materialChangeEvent;
	};

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
	bool MaterialPack::hasMaterialOf() const noexcept
	{
		return hasMaterialOf(typeid($M));
	}

	template <std::derived_from<Material> $M>
	void MaterialPack::setMaterial(
		std::shared_ptr<Material const> pMaterial) noexcept
	{
		setMaterial(typeid(*pMaterial), pMaterial);
	}

	constexpr Infra::EventView<MaterialPack const *, std::type_index, Material const *, Material const *> &
		MaterialPack::getMaterialChangeEvent() const noexcept
	{
		return __materialChangeEvent;
	}
}