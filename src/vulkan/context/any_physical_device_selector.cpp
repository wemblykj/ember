#include "any_physical_device_selector.h"

#include <algorithm>

namespace ember::graphics::vulkan {

std::optional<DeviceSelectionResult> AnyPhysicalDeviceSelector::select(VkInstance instance)
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount == 0) {
		return std::nullopt; // No devices found
	}

	deviceCount = 1; // We only need one device for selection
	std::vector<VkPhysicalDevice> devices(deviceCount);
	if (vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data()) != VK_SUCCESS) {
		return std::nullopt; // Failed to enumerate devices
	}

	auto device = devices.front();
	DeviceSelectionResult result { .physicalDevice = device };

	vkGetPhysicalDeviceFeatures(device, &result.features); // Query device features (not used in this simple selector)

	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	if (extensionCount > 0) {
		std::vector<VkExtensionProperties> available(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, available.data());

		std::ranges::transform(available, std::inserter(result.extensions, result.extensions.end()),
			[](const VkExtensionProperties& ext) { return ext.extensionName; });
	}
	
	return result;
}

PhysicalDeviceSelectorPtr createAnyPhysicalDeviceSelector()
{
	return std::move(std::make_unique<AnyPhysicalDeviceSelector>());
}

} // namespace ember::graphics::vulkan