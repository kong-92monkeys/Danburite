#include "MaterialDescriptorManager.h"

namespace Render
{
	MaterialDescriptorManager::MaterialDescriptorManager(
		VK::Device &device,
		ResourcePool &resourcePool,
		std::unordered_map<std::type_index, uint32_t> const &materialTypeIds) :
		__device			{ device },
		__resourcePool		{ resourcePool },
		__materialTypeIds	{ materialTypeIds }
	{
		__createDescSetLayout();
		__createDescPool();
		__createDescSets();

		__pMaterialUpdateListener = Infra::EventListener<Material const *>::bind(
			&MaterialDescriptorManager::__onMaterialUpdated, this, std::placeholders::_1);
	}

	void MaterialDescriptorManager::__createDescSetLayout()
	{
		std::vector<VkDescriptorSetLayoutBinding> bindings;

		for (auto const &[typeIndex, typeId] : __materialTypeIds)
		{
			auto &binding{ bindings.emplace_back() };
			binding.binding				= typeId;
			binding.descriptorType		= VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			binding.descriptorCount		= 1U;
			binding.stageFlags			= VkShaderStageFlagBits::VK_SHADER_STAGE_ALL;
		}

		VkDescriptorSetLayoutCreateInfo const createInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO },
			.bindingCount	{ static_cast<uint32_t>(bindings.size()) },
			.pBindings		{ bindings.data() }
		};

		__pDescSetLayout = std::make_unique<VK::DescriptorSetLayout>(__device, createInfo);
	}

	void MaterialDescriptorManager::__createDescPool()
	{
		std::vector<VkDescriptorPoolSize> poolSizes;

		poolSizes.emplace_back(
			VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			static_cast<uint32_t>(__materialTypeIds.size() * __descSets.size()));

		poolSizes.emplace_back(
			VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
			static_cast<uint32_t>(__descSets.size()));

		VkDescriptorPoolCreateInfo const createInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO },
			.maxSets		{ static_cast<uint32_t>(__descSets.size()) },
			.poolSizeCount	{ static_cast<uint32_t>(poolSizes.size()) },
			.pPoolSizes		{ poolSizes.data() }
		};

		__pDescPool = std::make_unique<VK::DescriptorPool>(__device, createInfo);
	}

	void MaterialDescriptorManager::__createDescSets()
	{
		std::array<VkDescriptorSetLayout, Constants::MAX_IN_FLIGHT_FRAME_COUNT> layoutHandles;
		layoutHandles.fill(__pDescSetLayout->getHandle());

		VkDescriptorSetAllocateInfo const allocInfo
		{
			.sType					{ VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO },
			.descriptorPool			{ __pDescPool->getHandle() },
			.descriptorSetCount		{ static_cast<uint32_t>(__descSets.size()) },
			.pSetLayouts			{ layoutHandles.data() }
		};

		std::array<VkDescriptorSet, Constants::MAX_IN_FLIGHT_FRAME_COUNT> setHandles{ };
		__device.vkAllocateDescriptorSets(&allocInfo, setHandles.data());

		for (size_t iter{ }; iter < setHandles.size(); ++iter)
			__descSets[iter] = std::make_unique<VK::DescriptorSet>(setHandles[iter]);
	}

	void MaterialDescriptorManager::__validateMaterialHostBuffer(
		Material const *pMaterial) noexcept
	{

	}

	void MaterialDescriptorManager::__validateMaterialBuffer()
	{

	}

	void MaterialDescriptorManager::__onMaterialUpdated(
		Material const *pMaterial) noexcept
	{
		__validateMaterialHostBuffer(pMaterial);
		__materialBufferInvalidated = true;
		_invalidate();
	}
}