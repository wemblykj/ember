#pragma once

#include <vulkan/context/physical_device_selector.hpp>

#include <optional>

namespace ember::vulkan {

class AnyPhysicalDeviceSelector : public PhysicalDeviceSelector {

private:
	std::optional<PhysicalDevicePtr> select(const PhysicalDevices& candidates) override;
};

} // namespace ember::vulkan