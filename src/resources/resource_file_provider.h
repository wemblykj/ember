#pragma once

#include <memory>
#include <string>
#include <vector>

namespace ember::resources {

/**
 * @brief Abstracts reading raw resource/asset data from an underlying source.
 *
 * Implementations may read from the native filesystem, a packed archive,
 * embedded binary resources, or any other backing store. Consumers (such as
 * shader compilers) should depend on this interface rather than directly
 * using std::fstream/std::filesystem.
 */
class ResourceFileProvider {
public:
    virtual ~ResourceFileProvider() = default;

    /**
     * @brief Reads the entire contents of a resource as text.
     * @param path Logical resource path (interpretation is provider-specific).
     * @return File contents, or std::nullopt if the resource could not be found/read.
     */
    virtual std::optional<std::string> readText(std::string_view path) const = 0;

    /**
     * @brief Reads the entire contents of a resource as binary data.
     */
    virtual std::optional<std::vector<std::byte>> readBinary(std::string_view path) const = 0;

    /**
     * @brief Returns true if the resource exists and is readable.
     */
    virtual bool exists(std::string_view path) const = 0;
};

using ResourceFileProviderPtr = std::shared_ptr<ResourceFileProvider>;

} // namespace ember::resources