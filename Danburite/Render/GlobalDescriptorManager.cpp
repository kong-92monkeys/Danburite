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
		__pMaterialBufferBuilderInvalidateListener =
			Infra::EventListener<MaterialBufferBuilder *>::bind(
				&GlobalDescriptorManager::__onMaterialBufferBuilderInvalidated, this, std::placeholders::_1);

		__createMaterialsDescSetLayout();
		__createSampledImagesDescSetLayout();

		__createMaterialsDescPool();
		__createSampledImagesDescPool();

		__allocateMaterialsDescSets();
		__allocateSampledImagesDescSets();

		__createMaterialBufferBuilders();
	}

	GlobalDescriptorManager::~GlobalDescriptorManager() noexcept
	{
		__materialBufferBuilders.clear();

		__pSampledImagesDescPool = nullptr;
		__pMaterialsDescPool = nullptr;

		__pSampledImagesDescSetLayout = nullptr;
		__pMaterialsDescSetLayout = nullptr;
	}

	void GlobalDescriptorManager::addMaterial(
		Material const *const pMaterial)
	{
		__materialBufferBuilders.at(typeid(*pMaterial))->addMaterial(pMaterial);
		_invalidate();
	}

	void GlobalDescriptorManager::removeMaterial(
		Material const *pMaterial)
	{
		__materialBufferBuilders.at(typeid(*pMaterial))->removeMaterial(pMaterial);
	}

	uint32_t GlobalDescriptorManager::getMaterialIdOf(
		Material const *pMaterial) const noexcept
	{
		return __materialBufferBuilders.at(typeid(*pMaterial))->getIdOf(pMaterial);
	}

	void GlobalDescriptorManager::_onValidate()
	{
		for (auto const pBuilder : __invalidatedMaterialBufferBuilders)
			pBuilder->validate();

		__validateDescSet();

		__invalidatedMaterialBufferBuilders.clear();
	}

	void GlobalDescriptorManager::__createMaterialsDescSetLayout()
	{
		std::vector<VkDescriptorBindingFlags> bindingFlags;
		std::vector<VkDescriptorSetLayoutBinding> bindings;

		for (auto const &[typeIndex, location] : __bindingInfo.materialBufferLocations)
		{
			auto &materialBufferBinding				{ bindings.emplace_back() };
			materialBufferBinding.binding			= location;
			materialBufferBinding.descriptorType	= VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			materialBufferBinding.descriptorCount	= 1U;
			materialBufferBinding.stageFlags		= VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;
			
			bindingFlags.emplace_back(0U);
		}

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

		__pMaterialsDescSetLayout = std::make_unique<VK::DescriptorSetLayout>(__device, createInfo);
	}

	void GlobalDescriptorManager::__createSampledImagesDescSetLayout()
	{
		auto const &deviceLimits{ __physicalDevice.getProps().p10->limits };

		std::vector<VkDescriptorBindingFlags> bindingFlags;
		std::vector<VkDescriptorSetLayoutBinding> bindings;

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

		__pSampledImagesDescSetLayout = std::make_unique<VK::DescriptorSetLayout>(__device, createInfo);
	}

	void GlobalDescriptorManager::__createMaterialsDescPool()
	{
		uint32_t const descSetCount				{ static_cast<uint32_t>(__materialsDescSets.size()) };
		uint32_t const materialTypeCount		{ static_cast<uint32_t>(__bindingInfo.materialBufferLocations.size()) };
		uint32_t const materialBufferPoolSize	{ materialTypeCount * descSetCount };

		std::vector<VkDescriptorPoolSize> poolSizes;

		if (materialBufferPoolSize)
		{
			poolSizes.emplace_back(
				VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				materialBufferPoolSize);
		}

		if (poolSizes.empty())
			return;

		VkDescriptorPoolCreateInfo const createInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO },
			.maxSets		{ descSetCount },
			.poolSizeCount	{ static_cast<uint32_t>(poolSizes.size()) },
			.pPoolSizes		{ poolSizes.data() }
		};

		__pMaterialsDescPool = std::make_shared<VK::DescriptorPool>(__device, createInfo);
	}

	void GlobalDescriptorManager::__createSampledImagesDescPool()
	{
		auto const &deviceLimits				{ __physicalDevice.getProps().p10->limits };
		uint32_t const descSetCount				{ static_cast<uint32_t>(__materialsDescSets.size()) };
		uint32_t const sampledImagePoolSize		{ __sampledImageDescCount * descSetCount };

		if (deviceLimits.maxPerStageDescriptorSampledImages < sampledImagePoolSize)
			throw std::runtime_error{ "The sampledImagePoolSize is overflowed." };

		VkDescriptorPoolSize const poolSize
		{
			.type				{ VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE },
			.descriptorCount	{ sampledImagePoolSize }
		};

		VkDescriptorPoolCreateInfo const createInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO },
			.maxSets		{ descSetCount },
			.poolSizeCount	{ 1U },
			.pPoolSizes		{ &poolSize }
		};

		__pSampledImagesDescPool = std::make_shared<VK::DescriptorPool>(__device, createInfo);
	}

	void GlobalDescriptorManager::__allocateMaterialsDescSets()
	{
		uint32_t const descSetCount{ static_cast<uint32_t>(__materialsDescSets.size()) };

		std::vector<VkDescriptorSetLayout> layoutHandles;

		for (uint32_t iter{ }; iter < descSetCount; ++iter)
			layoutHandles.emplace_back(__pMaterialsDescSetLayout->getHandle());

		VkDescriptorSetAllocateInfo const allocInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO },
			.descriptorPool		{ __pMaterialsDescPool->getHandle() },
			.descriptorSetCount	{ descSetCount },
			.pSetLayouts		{ layoutHandles.data() }
		};

		__device.vkAllocateDescriptorSets(&allocInfo, __materialsDescSets.data());
	}

	void GlobalDescriptorManager::__allocateSampledImagesDescSets()
	{
		uint32_t const descSetCount{ static_cast<uint32_t>(__sampledImagesDescSets.size()) };

		std::vector<uint32_t> sampledImageDescCounts;
		std::vector<VkDescriptorSetLayout> layoutHandles;

		for (uint32_t iter{ }; iter < descSetCount; ++iter)
		{
			sampledImageDescCounts.emplace_back(__sampledImageDescCount);
			layoutHandles.emplace_back(__pSampledImagesDescSetLayout->getHandle());
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
			.descriptorPool		{ __pSampledImagesDescPool->getHandle() },
			.descriptorSetCount	{ descSetCount },
			.pSetLayouts		{ layoutHandles.data() }
		};

		__device.vkAllocateDescriptorSets(&allocInfo, __sampledImagesDescSets.data());
	}

	void GlobalDescriptorManager::__createMaterialBufferBuilders()
	{
		for (auto const &[type, _] : __bindingInfo.materialBufferLocations)
		{
			auto pBuilder{ std::make_unique<MaterialBufferBuilder>(__resourcePool) };
			pBuilder->getInvalidateEvent() += __pMaterialBufferBuilderInvalidateListener;

			__materialBufferBuilders[type] = std::move(pBuilder);
		}
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
				getMaterialsDescSet(), __bindingInfo.materialBufferLocations.at(type), 0U, 1U,
				VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, &bufferInfo);
		}
	}

	void GlobalDescriptorManager::__growSampledImagesDescCount()
	{
		__deferredDeleter.reserve(std::move(__pSampledImagesDescPool));

		__sampledImageDescCount <<= 1U;

		__createSampledImagesDescPool();
		__allocateSampledImagesDescSets();
	}

	void GlobalDescriptorManager::__onMaterialBufferBuilderInvalidated(
		MaterialBufferBuilder *const pBuilder) noexcept
	{
		__invalidatedMaterialBufferBuilders.emplace(pBuilder);
		_invalidate();
	}
}