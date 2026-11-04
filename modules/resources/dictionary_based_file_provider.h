#pragma once

#include <istream>
#include <map>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <resource_file_provider.h>

namespace ember::resources {

template<typename ValueType = nullptr_t>
struct ContentConverter { 
	static_assert(sizeof(ValueType) == 0, "ContentConverter must be specialized for the type T");
};

template<>
struct ContentConverter<const std::span<std::byte>> {
	static std::unique_ptr<std::istream> toStream(const std::span<std::byte>& data) {
		// TODO: Consider how to avoid copying the data, but std::istringstream requires a std::string, so we need to copy the data here.
		return std::make_unique<std::istringstream>(std::string(reinterpret_cast<const char*>(data.data()), data.size()));
	}
};

template<>
struct ContentConverter<std::vector<std::byte>> {
	static std::unique_ptr<std::istream> toStream(const std::vector<std::byte>& data) {
		// TODO: Consider how to avoid copying the data, but std::istringstream requires a std::string, so we need to copy the data here.
		return std::make_unique<std::istringstream>(std::string(reinterpret_cast<const char*>(data.data()), data.size()));
	}
};

template<>
struct ContentConverter<std::string_view> {
	static std::unique_ptr<std::istream> toStream(const std::string_view sv) {
        // TODO: Consider how to avoid copying the data, but std::istringstream requires a std::string, so we need to copy the data here.
		return std::make_unique<std::istringstream>(std::string(sv));
	}
};
        
/**
 * @brief Reads resources directly from the native OS filesystem.
 */
template<typename ValueType>
class DictionaryBasedFileProvider : public resources::ResourceFileProvider {
public:
    DictionaryBasedFileProvider(std::string_view root, std::map<std::string, ValueType> fileDictionary = {});

    std::unique_ptr<std::istream> openText(std::string_view path) const override;
    std::unique_ptr<std::istream> openBinary(std::string_view path) const override;
    bool exists(std::string_view path) const override;

private:
    std::optional<std::string> resolvePath(std::string_view path) const;

    std::string_view root_;
	std::map<std::string, ValueType> fileDictionary_;
};

template<typename ValueType>
DictionaryBasedFileProvider<ValueType>::DictionaryBasedFileProvider(std::string_view root, std::map<std::string, ValueType> fileDictionary)
    : root_(root), fileDictionary_(std::move(fileDictionary)) {
}

template<typename ValueType>
std::unique_ptr<std::istream> DictionaryBasedFileProvider<ValueType>::openText(std::string_view path) const {
    const auto resolvedPath = resolvePath(path);
    if (!resolvedPath.has_value())
        return std::make_unique<std::istringstream>(""); // Return an empty stream if the file cannot be opened

	const auto it = fileDictionary_.find(*resolvedPath);

	if (it == fileDictionary_.end())
		return std::make_unique<std::istringstream>(""); // Return an empty stream if the file cannot be opened

    return ContentConverter<ValueType>::toStream(it->second);
}

template<typename ValueType>
std::unique_ptr<std::istream> DictionaryBasedFileProvider<ValueType>::openBinary(std::string_view path) const {
    const auto resolvedPath = resolvePath(path);
    if (!resolvedPath.has_value())
        return std::make_unique<std::istringstream>(""); // Return an empty stream if the file cannot be opened

    const auto it = fileDictionary_.find(*resolvedPath);

    if (it == fileDictionary_.end())
        return std::make_unique<std::istringstream>(""); // Return an empty stream if the file cannot be opened

    return ContentConverter<ValueType>::toStream(it->second);
}

template<typename ValueType>
bool DictionaryBasedFileProvider<ValueType>::exists(std::string_view path) const {
    const auto resolvedPath = resolvePath(path);
    if (!resolvedPath.has_value())
        return false;

    return fileDictionary_.contains(*resolvedPath);
}

template<typename ValueType>
std::optional<std::string> DictionaryBasedFileProvider<ValueType>::resolvePath(std::string_view path) const
{
    if (!path.starts_with(root_))
        return std::nullopt;

    return std::string(path.substr(root_.size()));
}

} // namespace ember::resources