#include "vulkan_instance.h"

#include <vulkan/vulkan.h>

namespace ember::graphics::vulkan {

VulkanInstance::VulkanInstance(VkInstance instance) 
	: instance_(instance) {
}

VulkanInstance::~VulkanInstance() {
    instance_ = VK_NULL_HANDLE;
}

} // namespace ember::graphics::vulkan