#include "GlobalDescriptorManager.h"
#include <stdexcept>

namespace Render
{
	GlobalDescriptorManager::GlobalDescriptorManager(
		VK::PhysicalDevice &physicalDevice,
		VK::Device &device,
		Infra::DeferredDeleter &deferredDeleter,
		ResourcePool &resourcePool,
		std::unordered_map<std::type_index, uint32_t> const &materialTypeIds) :
		__physicalDevice	{ physicalDevice },
		__device			{ device },
		__deferredDeleter	{ deferredDeleter },
		__resourcePool		{ resourcePool },
		__materialTypeIds	{ materialTypeIds }
	{
		__createDescSetLayout();
		__createDescPool();
		__allocateDescSets();
	}

	GlobalDescriptorManager::~GlobalDescriptorManager() noexcept
	{
		__pDescPool = nullptr;
		__pDescSetLayout = nullptr;
	}

	void GlobalDescriptorManager::__createDescSetLayout()
	{
		auto const &deviceLimits{ __physicalDevice.getProps().p10->limits };

		std::vector<VkDescriptorBindingFlags> bindingFlags;
		std::vector<VkDescriptorSetLayoutBinding> bindings;

		for (auto const &[typeIndex, typeId] : __materialTypeIds)
		{
			auto &materialBufferBinding				{ bindings.emplace_back() };
			materialBufferBinding.binding			= (typeId + 1U);
			materialBufferBinding.descriptorType	= VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			materialBufferBinding.descriptorCount	= 1U;
			materialBufferBinding.stageFlags		= VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;
			
			bindingFlags.emplace_back(0U);
		}

		auto &sampledImageBinding				{ bindings.emplace_back() };
		sampledImageBinding.binding				= 0U;
		sampledImageBinding.descriptorType		= VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		sampledImageBinding.descriptorCount		= deviceLimits.maxPerStageDescriptorSampledImages;
		sampledImageBinding.stageFlags			= VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;

		bindingFlags.emplace_back(
			VkDescriptorBindingFlagBits::VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT |
			VkDescriptorBindingFlagBits::VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);

		VkDescriptorSetLayoutBindingFlagsCreateInfo const flagInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO },
			.bindingCount	{ static_cast<uint32_t>(bindingFlags.size()) },
			.pBindingFlags	{ bindingFlags.data() }
		};

		VkDescriptorSetLayoutCreateInfo const createInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO },
			.pNext			{ &flagInfo },
			.bindingCount	{ static_cast<uint32_t>(bindings.size()) },
			.pBindings		{ bindings.data() }
		};

		__pDescSetLayout = std::make_unique<VK::DescriptorSetLayout>(__device, createInfo);
	}

	void GlobalDescriptorManager::__createDescPool()
	{
		auto const &deviceLimits				{ __physicalDevice.getProps().p10->limits };

		uint32_t const descSetCount				{ static_cast<uint32_t>(__descSets.size()) };
		uint32_t const materialBufferPoolSize	{ static_cast<uint32_t>(__materialTypeIds.size()) * descSetCount };
		uint32_t const sampledImagePoolSize		{ __sampledImageDescCount * descSetCount };

		if (deviceLimits.maxPerStageDescriptorSampledImages < sampledImagePoolSize)
			throw std::runtime_error{ "The sampledImagePoolSize is overflowed." };

		std::vector<VkDescriptorPoolSize> poolSizes;

		/*poolSizes.emplace_back(
			VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			materialBufferPoolSize);*/

		poolSizes.emplace_back(
			VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
			sampledImagePoolSize);

		VkDescriptorPoolCreateInfo const createInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO },
			.maxSets		{ descSetCount },
			.poolSizeCount	{ static_cast<uint32_t>(poolSizes.size()) },
			.pPoolSizes		{ poolSizes.data() }
		};

		__pDescPool = std::make_shared<VK::DescriptorPool>(__device, createInfo);
	}

	void GlobalDescriptorManager::__allocateDescSets()
	{
		uint32_t const descSetCount{ static_cast<uint32_t>(__descSets.size()) };

		std::vector<uint32_t> sampledImageDescCounts;
		std::vector<VkDescriptorSetLayout> layoutHandles;

		for (uint32_t iter{ }; iter < descSetCount; ++iter)
		{
			sampledImageDescCounts.emplace_back(__sampledImageDescCount);
			layoutHandles.emplace_back(__pDescSetLayout->getHandle());
		}

		VkDescriptorSetVariableDescriptorCountAllocateInfo const descCountInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO },
			.descriptorSetCount	{ descSetCount },
			.pDescriptorCounts	{ sampledImageDescCounts.data() }
		};

		VkDescriptorSetAllocateInfo const allocInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO },
			.pNext				{ &descCountInfo },
			.descriptorPool		{ __pDescPool->getHandle() },
			.descriptorSetCount	{ descSetCount },
			.pSetLayouts		{ layoutHandles.data() }
		};

		__device.vkAllocateDescriptorSets(&allocInfo, __descSets.data());
	}

	void GlobalDescriptorManager::__growSampledImageDescCount()
	{
		__deferredDeleter.reserve(std::move(__pDescPool));

		__sampledImageDescCount <<= 1U;
		__createDescPool();
		__allocateDescSets();
	}
}