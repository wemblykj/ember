#pragma once

#ifdef EMBER_OPENXR_ENABLED

#include <memory>

namespace ember::graphics::openxr {

/**
 * OpenXR context - manages the OpenXR instance and system
 * Handles low-level OpenXR API initialization
 */
class OpenXRContext {
public:
    OpenXRContext();
    ~OpenXRContext();

    bool initialize(const std::string& appName);
    void shutdown();

    // Getters for low-level OpenXR objects
    void* getInstance() const { return instance_; }
    uint64_t getSystemId() const { return systemId_; }

private:
    void* instance_ = nullptr;
    uint64_t systemId_ = 0;
    bool initialized_ = false;
};

}  // namespace ember::graphics::openxr

#endif // EMBER_OPENXR_ENABLED
