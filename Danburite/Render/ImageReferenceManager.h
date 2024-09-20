#pragma once

#include "../Infra/IdAllocator.h"
#include "../Infra/Updatable.h"
#include "../Vulkan/ImageView.h"

namespace Render
{
	class ImageReferenceManager : public Infra::Unique, public Infra::Updatable<ImageReferenceManager>
	{
	public:
		void addImage(
			VK::ImageView const *pImage);

		void removeImage(
			VK::ImageView const *pImage);

		[[nodiscard]]
		uint32_t getIdOf(
			VK::ImageView const *pImage) const noexcept;

		[[nodiscard]]
		constexpr std::vector<VkDescriptorImageInfo> const &getDescInfos() const noexcept;

	private:
		Infra::IdAllocator<uint32_t> __idAllocator;
		std::unordered_map<VK::ImageView const *, std::pair<size_t, uint32_t>> __refIdMap;

		std::vector<VkDescriptorImageInfo> __descInfos;

		void __registerImage(
			VK::ImageView const *pImage);

		void __unregisterImage(
			VK::ImageView const *pImage);

		void __updateDescInfo(
			VK::ImageView const *pImage) noexcept;
	};

	constexpr std::vector<VkDescriptorImageInfo> const &ImageReferenceManager::getDescInfos() const noexcept
	{
		return __descInfos;
	}
}