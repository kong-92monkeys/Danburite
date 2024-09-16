#include "GlobalDescriptorManager.h"
#include "Constants.h"
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
		auto const &deviceLimits{ __physicalDevice.getProps().p10->limits };

		uint32_t const sampledImagePoolSize
		{
			static_cast<uint32_t>(
				__sampledImageDescCount * Constants::MAX_IN_FLIGHT_FRAME_COUNT)
		};

		if (deviceLimits.maxPerStageDescriptorSampledImages < sampledImagePoolSize)
			throw std::runtime_error{ "The sampledImagePoolSize is overflowed." };

		std::vector<VkDescriptorPoolSize> poolSizes;

		/*poolSizes.emplace_back(
			VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			static_cast<uint32_t>(__materialTypeIds.size() * Constants::MAX_IN_FLIGHT_FRAME_COUNT));*/

		poolSizes.emplace_back(
			VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
			sampledImagePoolSize);

		VkDescriptorPoolCreateInfo const createInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO },
			.maxSets		{ static_cast<uint32_t>(Constants::MAX_IN_FLIGHT_FRAME_COUNT) },
			.poolSizeCount	{ static_cast<uint32_t>(poolSizes.size()) },
			.pPoolSizes		{ poolSizes.data() }
		};

		__pDescPool = std::make_unique<VK::DescriptorPool>(__device, createInfo);
	}
}