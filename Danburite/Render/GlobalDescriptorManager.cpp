#include "GlobalDescriptorManager.h"

namespace Render
{
	GlobalDescriptorManager::GlobalDescriptorManager(
		VK::Device &device,
		Infra::DeferredDeleter &deferredDeleter,
		ResourcePool &resourcePool,
		std::unordered_map<std::type_index, uint32_t> const &materialTypeIds) :
		__device			{ device },
		__deferredDeleter	{ deferredDeleter },
		__resourcePool		{ resourcePool },
		__materialTypeIds	{ materialTypeIds }
	{
		__createDescSetLayout();
	}

	GlobalDescriptorManager::~GlobalDescriptorManager() noexcept
	{
		__pDescSetLayout = nullptr;
	}

	void GlobalDescriptorManager::__createDescSetLayout()
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
}