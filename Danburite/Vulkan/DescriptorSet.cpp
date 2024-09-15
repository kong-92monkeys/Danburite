#include "DescriptorSet.h"
#include <stdexcept>

namespace VK
{
	DescriptorSet::DescriptorSet(
		VkDescriptorSet const handle) noexcept :
		Handle	{ handle }
	{}
}