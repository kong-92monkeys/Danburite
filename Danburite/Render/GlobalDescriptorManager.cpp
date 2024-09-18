#include "GlobalDescriptorManager.h"
#include <stdexcept>

namespace Render
{
	GlobalDescriptorManager::GlobalDescriptorManager(
		VK::PhysicalDevice &physicalDevice,
		VK::Device &device,
		Infra::DeferredDeleter &deferredDeleter,
		Dev::DescriptorUpdater &descriptorUpdater,
		ResourcePool &resourcePool,
		BindingInfo const &bindingInfo) :
		__physicalDevice	{ physicalDevice },
		__device			{ device },
		__deferredDeleter	{ deferredDeleter },
		__descriptorUpdater	{ descriptorUpdater },
		__resourcePool		{ resourcePool },
		__bindingInfo		{ bindingInfo }
	{
		__createDescSetLayout();
		__createDescPool();
		__allocateDescSets();
		__createMaterialBufferBuilders();
	}

	GlobalDescriptorManager::~GlobalDescriptorManager() noexcept
	{
		__materialBufferBuilders.clear();
		__pDescPool = nullptr;
		__pDescSetLayout = nullptr;
	}

	void GlobalDescriptorManager::registerMaterial(
		Material const *const pMaterial)
	{
		__materialBufferBuilders.at(typeid(*pMaterial))->registerMaterial(pMaterial);
		_invalidate();
	}

	void GlobalDescriptorManager::unregisterMaterial(
		Material const *pMaterial)
	{
		__materialBufferBuilders.at(typeid(*pMaterial))->unregisterMaterial(pMaterial);
	}

	uint32_t GlobalDescriptorManager::getIdOf(
		Material const *pMaterial) const noexcept
	{
		return __materialBufferBuilders.at(typeid(*pMaterial))->getIdOf(pMaterial);
	}

	void GlobalDescriptorManager::_onValidate()
	{
		__validateMaterialBufferBuilders();
		__validateDescSet();
	}

	void GlobalDescriptorManager::__createDescSetLayout()
	{
		auto const &deviceLimits{ __physicalDevice.getProps().p10->limits };

		std::vector<VkDescriptorBindingFlags> bindingFlags;
		std::vector<VkDescriptorSetLayoutBinding> bindings;

		for (auto const &[typeIndex, binding] : __bindingInfo.materialBufferBindings)
		{
			auto &materialBufferBinding				{ bindings.emplace_back() };
			materialBufferBinding.binding			= binding;
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
		uint32_t const materialTypeCount		{ static_cast<uint32_t>(__bindingInfo.materialBufferBindings.size()) };
		uint32_t const materialBufferPoolSize	{ materialTypeCount * descSetCount };
		uint32_t const sampledImagePoolSize		{ __sampledImageDescCount * descSetCount };

		if (deviceLimits.maxPerStageDescriptorSampledImages < sampledImagePoolSize)
			throw std::runtime_error{ "The sampledImagePoolSize is overflowed." };

		std::vector<VkDescriptorPoolSize> poolSizes;

		if (materialBufferPoolSize)
		{
			poolSizes.emplace_back(
				VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				materialBufferPoolSize);
		}

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

	void GlobalDescriptorManager::__createMaterialBufferBuilders()
	{
		for (auto const &[type, _] : __bindingInfo.materialBufferBindings)
			__materialBufferBuilders[type] = std::make_unique<MaterialBufferBuilder>(__resourcePool);
	}

	void GlobalDescriptorManager::__validateMaterialBufferBuilders()
	{
		for (auto const &[_, pBuilder] : __materialBufferBuilders)
			pBuilder->validate();
	}

	void GlobalDescriptorManager::__validateDescSet()
	{
		__advanceDescSet();

		for (auto const &[type, pBuilder] : __materialBufferBuilders)
		{
			auto const &materialBuffer{ pBuilder->getMaterialBuffer() };

			VkDescriptorBufferInfo const bufferInfo
			{
				.buffer	{ materialBuffer.getHandle() },
				.offset	{ 0ULL },
				.range	{ materialBuffer.getSize() }
			};

			__descriptorUpdater.addBufferInfos(
				getDescSet(), __bindingInfo.materialBufferBindings.at(type), 0U, 1U,
				VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, &bufferInfo);
		}
	}

	void GlobalDescriptorManager::__growSampledImageDescCount()
	{
		__deferredDeleter.reserve(std::move(__pDescPool));

		__sampledImageDescCount <<= 1U;
		__descSetCursor = 0ULL;

		__createDescPool();
		__allocateDescSets();
	}
}