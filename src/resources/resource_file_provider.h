#pragma once

#include <algorithm>
#include <istream>
#include <memory>
#include <sstream>
#include <string>

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
	 * @return File contents as a std::istream.
	 */
    virtual std::unique_ptr<std::istream> openText(std::string_view path) const = 0;

    /**
     * @brief Reads the entire contents of a resource as binary data.
     * @param path Logical resource path (interpretation is provider-specific).
     * @return File contents as a std::istream.
     */
    virtual std::unique_ptr<std::istream> openBinary(std::string_view path) const = 0;

    /**
     * @brief Returns true if the resource exists and is readable.
     */
    virtual bool exists(std::string_view path) const = 0;

	/**
	 * @brief Reads the entire contents of a resource as text and returns it as a string.
	 * @param path Logical resource path (interpretation is provider-specific).
	 * @return File contents as a std::string.
	 */
    std::optional<std::string> readText(std::string_view path) const {
        if (!exists(path))
            return std::nullopt;

        auto stream = openText(path);
        std::stringstream buffer;
		buffer << stream->rdbuf();
        return buffer.str();
    }

	/**
	 * @brief Reads the entire contents of a resource as binary data and returns it as a vector of bytes.
	 * @param path Logical resource path (interpretation is provider-specific).
	 * @return File contents as a std::vector<std::byte>.
	 */
	std::optional<std::vector<std::byte>> readBinary(std::string_view path) const {
        if (!exists(path))
            return std::nullopt;

        auto stream = openBinary(path);
        std::vector<std::byte> out;
        std::transform(std::istreambuf_iterator<char>(*stream),
            std::istreambuf_iterator<char>(),
            std::back_inserter(out),
            [](char c) { return std::byte(static_cast<unsigned char>(c)); });
        return out;
	}
};

using ResourceFileProviderPtr = std::shared_ptr<ResourceFileProvider>;

} // namespace ember::resources