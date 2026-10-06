#pragma once

#include "resource_file_provider.h"

namespace ember::core {

/**
 * @brief Reads resources directly from the native OS filesystem.
 */
class NativeFileProvider : public ResourceFileProvider {
public:
    std::optional<std::string> readText(const std::string& path) override;
    std::optional<std::vector<std::byte>> readBinary(const std::string& path) override;
    bool exists(const std::string& path) override;
};

} // namespace ember::core