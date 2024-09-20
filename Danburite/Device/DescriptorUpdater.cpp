#include "DescriptorUpdater.h"

namespace Dev
{
	DescriptorUpdater::DescriptorUpdater(
		VK::Device &device) noexcept :
		__device	{ device }
	{}

	void DescriptorUpdater::addInfos(
		VkDescriptorSet const hDescSet,
		uint32_t const dstBinding,
		uint32_t const dstArrayElement,
		uint32_t const descriptorCount,
		VkDescriptorType const descriptorType,
		VkDescriptorBufferInfo const *const pInfos) noexcept
	{
		auto &infos{ __getBufferInfos() };

		for (uint32_t infoIter{ }; infoIter < descriptorCount; ++infoIter)
			infos.emplace_back(pInfos[infoIter]);
		
		auto &write				{ __writes.emplace_back() };
		write.sType				= VkStructureType::VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet			= hDescSet;
		write.dstBinding		= dstBinding;
		write.dstArrayElement	= dstArrayElement;
		write.descriptorCount	= descriptorCount;
		write.descriptorType	= descriptorType;
		write.pBufferInfo		= infos.data();
	}

	void DescriptorUpdater::addInfos(
		VkDescriptorSet const hDescSet,
		uint32_t const dstBinding,
		uint32_t const dstArrayElement,
		uint32_t const descriptorCount,
		VkDescriptorType const descriptorType,
		VkDescriptorImageInfo const *const pInfos) noexcept
	{
		auto &infos{ __getImageInfos() };

		for (uint32_t infoIter{ }; infoIter < descriptorCount; ++infoIter)
			infos.emplace_back(pInfos[infoIter]);
		
		auto &write				{ __writes.emplace_back() };
		write.sType				= VkStructureType::VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet			= hDescSet;
		write.dstBinding		= dstBinding;
		write.dstArrayElement	= dstArrayElement;
		write.descriptorCount	= descriptorCount;
		write.descriptorType	= descriptorType;
		write.pImageInfo		= infos.data();
	}

	void DescriptorUpdater::update()
	{
		if (__writes.empty())
			return;

		__device.vkUpdateDescriptorSets(
			static_cast<uint32_t>(__writes.size()), __writes.data(), 0U, nullptr);

		__writes.clear();
		__bufferInfoCount = 0ULL;
	}

	std::vector<VkDescriptorBufferInfo> &DescriptorUpdater::__getBufferInfos() noexcept
	{
		if (__bufferInfoCount >= __bufferInfos.size())
			__bufferInfos.emplace_back();

		auto &retVal{ __bufferInfos[__bufferInfoCount] };
		++__bufferInfoCount;

		return retVal;
	}

	std::vector<VkDescriptorImageInfo> &DescriptorUpdater::__getImageInfos() noexcept
	{
		if (__imageInfoCount >= __imageInfos.size())
			__imageInfos.emplace_back();

		auto &retVal{ __imageInfos[__imageInfoCount] };
		++__imageInfoCount;

		return retVal;
	}
}