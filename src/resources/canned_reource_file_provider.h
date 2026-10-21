#pragma once

#include <resource_file_provider.h>

namespace ember::resources {

/**
 * @brief Serves up canned resource files.
 */
class CannedResourceFileProvider : public ResourceFileProvider {
public:
    CannedResourceFileProvider();

    std::optional<std::string> readText(std::string_view path) const override
    std::optional<std::vector<std::byte>> readBinary(std::string_view path) const override;
    bool exists(std::string_view path) const override;

private:
};

} // namespace ember::resources