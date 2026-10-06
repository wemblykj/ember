#include "scoped_vulkan_instance.h"

#include <algorithm>

namespace ember::graphics::vulkan {

ScopedVulkanInstance::~ScopedVulkanInstance() {
	const auto& instance = get();
	if (instance != VK_NULL_HANDLE) {
		vkDestroyInstance(instance, nullptr);
	}
}

ExtensionSet getPlatformExtensions() {
    ExtensionSet extensions;

    // Platform-specific surface extension
#ifdef VK_USE_PLATFORM_WIN32_KHR
    extensions.insert(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
    extensions.insert(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XCBKHR)
    extensions.insert(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_METAL_EXT)
    extensions.insert(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
#endif

    return extensions;
}

VulkanInstancePtr createVulkanInstance(const ExtensionSet& requiredExtensions) {
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Ember Engine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Ember";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;

    ExtensionSet consolidatedExtensions = getPlatformExtensions();
    consolidatedExtensions.insert(requiredExtensions.begin(), requiredExtensions.end());

    // Instance create info
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    std::vector<const char*> enabledExtensions;
    std::ranges::transform(consolidatedExtensions, std::back_inserter(enabledExtensions), [](const std::string_view& ext) { return ext.data(); });
    createInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size());    
    createInfo.ppEnabledExtensionNames = enabledExtensions.data();

    // Validation layers
    std::vector<const char*> validationLayers;
#ifdef EMBER_DEBUG
    validationLayers.push_back("VK_LAYER_KHRONOS_validation");
    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
#else
    createInfo.enabledLayerCount = 0;
#endif

    VkInstance instance;
    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    if (result != VK_SUCCESS) {
        EMBER_LOG_ERROR("vkCreateInstance failed with code: {}", std::to_string(result));
		throw std::runtime_error("Failed to create Vulkan instance");
    }

	return std::make_shared<ScopedVulkanInstance>(instance);
}

} // namespace ember::graphics::vulkan