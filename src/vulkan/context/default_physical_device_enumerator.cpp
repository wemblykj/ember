#include "default_physical_device_selector.h"

#include <algorithm>

#include <vulkan/vulkan.h>

#include <logger.h>

namespace ember::vulkan {

std::vector<PhysicalDevicePtr> DefaultPhysicalDeviceEnumerator::enumerate(const VulkanInstancePtr& instance) {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance_, &deviceCount, nullptr);

	if (deviceCount == 0) {
		EMBER_LOG_ERROR("No physical devices found");
		return false;
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance_, &deviceCount, devices.data());

	std::vector<PhysicalDevice> physicalDevices;
	physicalDevices.reserve(deviceCount);
	for (const auto& device : devices) {
		physicalDevices.push_back(buiildPhysicalDevice(device));
	}

	return physicalDevices;
}

PhysicalDevice DefaultPhysicalDeviceEnumerator::buiildPhysicalDevice(VkPhysicalDevice device) {

	PhysicalDevice result{ .physicalDevice = device };

	if (!vkGetPhysicalDeviceProperties(device, &result.traits.properties)) {
		EMBER_LOG_ERROR("Failed to get physical device properties");
	}

	if (!vkGetPhysicalDeviceFeatures(device, &result.traits.features)) {
		EMBER_LOG_ERROR("Failed to get physical device features");
	}

	uint32_t extensionCount = 0;
	if (!vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr)) {
		EMBER_LOG_ERROR("Failed to enumerate device extension properties");
	}

	if (extensionCount > 0) {
		std::vector<VkExtensionProperties> available(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, available.data());

		result.extensions.reserve(extensionCount);
		std::ranges::transform(available, std::inserter(result.traits.extensions, result.traits.extensions.end()),
			[](const VkExtensionProperties& ext) { return ext.extensionName; });
	}
	
	uint32_t queueFamilyCount = 0;
	if (!vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr)) {
		EMBER_LOG_ERROR("Failed to get physical device queue family properties");
	}

	if (queueFamilyCount > 0) {
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		result.traits.queueFamilies.reserve(queueFamilyCount);
		for (uint32_t i = 0; i < queueFamilyCount; ++i) {
			const auto& qf = queueFamilies[i];
			result.traits.queueFamilies.push_back(QueueFamilyTraits{
				.queueFamilyIndex = i,
				.flags = qf.queueFlags,
				.queueCount = qf.queueCount
			});
		}
	}

	return result;
}