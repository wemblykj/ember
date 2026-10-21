#pragma once

#include <resource_file_provider.h>

namespace ember::application {

template<typename ValueType = nullptr_t>
struct ContentConverter { 
	std::static_assert(sizeof(ValueType) == 0, "ContentConverter must be specialized for the type T");
};

template<>
struct ContentConverter<std::byte> {
	static std::span<std::byte> toBytes(const std::vector<std::byte>& data) {
		return data;
	}

	static std::string_view toStringView(const std::vector<std::byte>& data) {
		return std::string_view(reinterpret_cast<const char*>(data.data()), data.size());
	}
};

template<>
struct ContentConverter<std::string_view> {
	static std::span<std::byte> toBytes(const std::string_view sv) {
		return std::span<std::byte>(reinterpret_cast<const std::byte*>(sv.data()), sv.size());
	}

	static std::string_view toStringView(std::string_view sv) {
		return sv;
	}
};
    
        
/**
 * @brief Reads resources directly from the native OS filesystem.
 */
template<typename ValueType = std::byte>
class DictionaryBasedFileProvider : public resources::ResourceFileProvider {
public:
    DictionaryBasedFileProvider(std::string_view root);

    std::optional<std::string> readText(std::string_view path) const override;
    std::optional<std::span<std::byte>> readBinary(std::string_view path) const override;
    bool exists(std::string_view path) const override;

private:
    std::optional<std::string_view> resolvePath(std::string_view path) const;

    std::string_view root_;
	std::map<std::string_view, std::vector<ValueType>> fileDictionary_;
};

template<typename ValueType>
DictionaryBasedFileProvider<ValueType>::DictionaryBasedFileProvider(std::string_view root)
    : root_(root) {
}

template<typename ValueType>
std::unique_ptr<std::istream> DictionaryBasedFileProvider<ValueType>::openText(std::string_view path) const {
    const auto resolvedPath = resolvePath(path);
    if (!resolvedPath.has_value())
        return std::make_unique<std::istringstream>(""); // Return an empty stream if the file cannot be opened

	const auto it = fileDictionary_.find(*resolvedPath);

	if (it == fileDictionary_.end())
		return std::make_unique<std::istringstream>(""); // Return an empty stream if the file cannot be opened

	return std::make_unique<std::istringstream>(ContentConverter<ValueType>::toStringView(it->second));
}

template<typename ValueType>
std::unique_ptr<std::istream> DictionaryBasedFileProvider<ValueType>::openBinary(std::string_view path) const {
    const auto resolvedPath = resolvePath(path);
    if (!resolvedPath.has_value())
        return std::make_unique<std::istringstream>(""); // Return an empty stream if the file cannot be opened

    const auto it = fileDictionary_.find(*resolvedPath);

    if (it == fileDictionary_.end())
        return std::make_unique<std::istringstream>(""); // Return an empty stream if the file cannot be opened

    return std::make_unique<std::istringstream>(ContentConverter<ValueType>::toStringView(it->second), std::ios::binary);
}

template<typename ValueType>
bool DictionaryBasedFileProvider<ValueType>::exists(std::string_view path) const {
    const auto resolvedPath = resolvePath(path);
    if (!resolvedPath.has_value())
        return false;

    return fileDictionary_.contains(*resolvedPath);
}

template<typename ValueType>
std::optional<std::string_view> DictionaryBasedFileProvider<ValueType>::resolvePath(std::string_view path) const
{
    if (!path.starts_with(root_))
        return std::nullopt;

    return path.substr(root_.size();
}

} // namespace ember::application