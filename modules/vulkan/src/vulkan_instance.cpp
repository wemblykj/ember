#include <vulkan/vulkan_instance.hpp>

#include <vulkan/vulkan.hpp>

namespace ember::vulkan {

VulkanInstance::VulkanInstance(VkInstance instance) 
	: instance_(instance) {
}

VulkanInstance::~VulkanInstance() {
    instance_ = VK_NULL_HANDLE;
}

} // namespace ember::vulkan