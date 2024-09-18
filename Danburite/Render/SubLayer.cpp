#include "SubLayer.h"

namespace Render
{
	SubLayer::SubLayer(
		VK::Device &device,
		VK::DescriptorSetLayout &descSetLayout,
		Infra::DeferredDeleter &deferredDeleter,
		Dev::DescriptorUpdater &descUpdater,
		ResourcePool &resourcePool,
		GlobalDescriptorManager &globalDescManager,
		Renderer const &renderer) noexcept :
		__device				{ device },
		__descSetLayout			{ descSetLayout },
		__deferredDeleter		{ deferredDeleter },
		__descUpdater			{ descUpdater },
		__resourcePool			{ resourcePool },
		__globalDescManager		{ globalDescManager },
		__renderer				{ renderer }
	{
		__createSubLayerDescSetPool();
		__allocSubLayerDescSets();

		__pObjectMeshChangeListener =
			Infra::EventListener<RenderObject const *, Mesh const *, Mesh const *>::bind(
				&SubLayer::__onObjectMeshChanged, this,
				std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

		__pObjectMaterialChangeListener =
			Infra::EventListener<RenderObject const *, uint32_t, std::type_index, Material const *, Material const *>::bind(
				&SubLayer::__onObjectMaterialChanged, this,
				std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);

		__pObjectInstanceCountChangeListener =
			Infra::EventListener<RenderObject const *, uint32_t, uint32_t>::bind(
				&SubLayer::__onObjectInstanceCountChanged, this,
				std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

		__pObjectDrawableChangeListener =
			Infra::EventListener<const RenderObject *, bool>::bind(
				&SubLayer::__onObjectDrawableChanged, this, std::placeholders::_1, std::placeholders::_2);
	}

	SubLayer::~SubLayer() noexcept
	{
		__deferredDeleter.reserve(std::move(__pDescPool));
	}

	void SubLayer::addRenderObject(
		RenderObject const *const pObject) noexcept
	{

	}

	void SubLayer::removeRenderObject(
		RenderObject const *const pObject) noexcept
	{

	}

	void SubLayer::__createSubLayerDescSetPool()
	{
		uint32_t const descSetCount{ static_cast<uint32_t>(__descSets.size()) };

		std::vector<VkDescriptorPoolSize> poolSizes;
		poolSizes.emplace_back(
			VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			descSetCount);

		VkDescriptorPoolCreateInfo const createInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO },
			.maxSets		{ descSetCount },
			.poolSizeCount	{ static_cast<uint32_t>(poolSizes.size()) },
			.pPoolSizes		{ poolSizes.data() }
		};

		__pDescPool = std::make_shared<VK::DescriptorPool>(__device, createInfo);
	}

	void SubLayer::__allocSubLayerDescSets()
	{
		uint32_t const descSetCount{ static_cast<uint32_t>(__descSets.size()) };

		std::vector<VkDescriptorSetLayout> layoutHandles;

		for (uint32_t iter{ }; iter < descSetCount; ++iter)
			layoutHandles.emplace_back(__descSetLayout.getHandle());

		VkDescriptorSetAllocateInfo const allocInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO },
			.descriptorPool		{ __pDescPool->getHandle() },
			.descriptorSetCount	{ descSetCount },
			.pSetLayouts		{ layoutHandles.data() }
		};

		__device.vkAllocateDescriptorSets(&allocInfo, __descSets.data());
	}

	void SubLayer::__onObjectMeshChanged(
		RenderObject const *const pObject,
		Mesh const *const pPrev,
		Mesh const *const pCur) noexcept
	{

	}

	void SubLayer::__onObjectMaterialChanged(
		RenderObject const *const pObject,
		uint32_t const instanceIndex,
		std::type_index const &type,
		Material const *const pPrev,
		Material const *const pCur) noexcept
	{

	}

	void SubLayer::__onObjectInstanceCountChanged(
		RenderObject const *const pObject,
		uint32_t const prev,
		uint32_t const cur) noexcept
	{

	}

	void SubLayer::__onObjectDrawableChanged(
		RenderObject const *const pObject,
		bool const cur) noexcept
	{

	}
}