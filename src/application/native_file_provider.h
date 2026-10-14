#pragma once

#include <resource_file_provider.h>

#include <filesystem>

namespace ember::application {

/**
 * @brief Reads resources directly from the native OS filesystem.
 */
class NativeFileProvider : public resources::ResourceFileProvider {
public:
    NativeFileProvider(const std::filesystem::path& root, bool enforceBoundary = true);

    std::optional<std::string> readText(std::string_view path) const override;
    std::optional<std::vector<std::byte>> readBinary(std::string_view path) const override;
    bool exists(std::string_view path) const override;

private:
    std::filesystem::path resolvePath(std::string_view path) const;

    std::filesystem::path root_;
    bool enforceBoundary_;
};

} // namespace ember::application