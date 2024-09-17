#include "DescriptorUpdater.h"

namespace Dev
{
	DescriptorUpdater::DescriptorUpdater(
		VK::Device &device) noexcept :
		__device	{ device }
	{}

	void DescriptorUpdater::addBufferInfos(
		VkDescriptorSet const hDescSet,
		uint32_t const dstBinding,
		uint32_t const dstArrayElement,
		uint32_t const descriptorCount,
		VkDescriptorType const descriptorType,
		VkDescriptorBufferInfo const *const pInfos) noexcept
	{
		auto &bufferInfos{ __getBufferInfos() };

		for (uint32_t infoIter{ }; infoIter < descriptorCount; ++infoIter)
			bufferInfos.emplace_back(pInfos[infoIter]);
		
		auto &write				{ __writes.emplace_back() };
		write.sType				= VkStructureType::VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet			= hDescSet;
		write.dstBinding		= dstBinding;
		write.dstArrayElement	= dstArrayElement;
		write.descriptorCount	= descriptorCount;
		write.descriptorType	= descriptorType;
		write.pBufferInfo		= bufferInfos.data();
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
}