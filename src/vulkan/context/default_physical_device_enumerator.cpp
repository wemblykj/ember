#include "default_physical_device_enumerator.h"

#include <algorithm>

#include <vulkan/vulkan.h>

#include <logger.h>

namespace ember::vulkan {

DefaultPhysicalDeviceEnumerator::DefaultPhysicalDeviceEnumerator(const VulkanInstancePtr& instance)
	: instance_(instance) {
}

std::vector<PhysicalDevicePtr> DefaultPhysicalDeviceEnumerator::enumerate() {
	uint32_t deviceCount = 0;
	if (vkEnumeratePhysicalDevices(instance_->get(), &deviceCount, nullptr) != VK_SUCCESS) {
		EMBER_LOG_ERROR("Failed to enumerate physical devices");
		return {};
	}

	if (deviceCount == 0) {
		EMBER_LOG_ERROR("No physical devices found");
		return {};
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	if (vkEnumeratePhysicalDevices(instance_->get(), &deviceCount, devices.data()) != VK_SUCCESS) {
		EMBER_LOG_ERROR("Failed to enumerate physical devices");
		return {};
	}

	std::vector<PhysicalDevicePtr> physicalDevices;
	physicalDevices.reserve(deviceCount);
	for (const auto& device : devices) {
		physicalDevices.emplace_back(buiildPhysicalDevice(device));
	}

	return physicalDevices;
}

PhysicalDevicePtr DefaultPhysicalDeviceEnumerator::buiildPhysicalDevice(VkPhysicalDevice device) {

	auto result = std::make_shared<PhysicalDevice>();
	result->handle = device;

	vkGetPhysicalDeviceProperties(device, &result->traits.properties);

	vkGetPhysicalDeviceFeatures(device, &result->traits.features);

	uint32_t extensionCount = 0;
	if (vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr) != VK_SUCCESS) {
		EMBER_LOG_ERROR("Failed to enumerate device extension properties");
	}

	if (extensionCount > 0) {
		std::vector<VkExtensionProperties> available(extensionCount);
		if (vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, available.data()) != VK_SUCCESS) {
			EMBER_LOG_ERROR("Failed to enumerate device extension properties");
		}

		std::ranges::transform(available, std::inserter(result->traits.extensions, result->traits.extensions.end()),
			[](const VkExtensionProperties& ext) { return ext.extensionName; });
	}

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	if (queueFamilyCount > 0) {
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		result->traits.queueFamilies.reserve(queueFamilyCount);
		for (uint32_t i = 0; i < queueFamilyCount; ++i) {
			const auto& qf = queueFamilies[i];
			result->traits.queueFamilies.push_back(QueueFamilyTraits{
				.queueFamilyIndex = i,
				.flags = qf.queueFlags,
				.queues = std::vector<QueueTraits>(qf.queueCount, QueueTraits{ 1.0f })
				});
		}
	}

	return result;
}

} // namespace ember::vulkan