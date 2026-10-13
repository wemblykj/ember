#include "native_file_provider.h"
#include <filesystem>
#include <fstream>

namespace ember::core {

std::optional<std::string> NativeFileProvider::readText(const std::string& path) {
    std::ifstream stream(path, std::ios::in | std::ios::binary);
    if (!stream) {
        return std::nullopt;
    }

    std::string contents(
        (std::istreambuf_iterator<char>(stream)),
        std::istreambuf_iterator<char>());
    return contents;
}

std::optional<std::vector<std::byte>> NativeFileProvider::readBinary(const std::string& path) {
    std::ifstream stream(path, std::ios::in | std::ios::binary | std::ios::ate);
    if (!stream) {
        return std::nullopt;
    }

    const auto size = static_cast<std::size_t>(stream.tellg());
    stream.seekg(0, std::ios::beg);

    std::vector<std::byte> buffer(size);
    stream.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(size));
    return buffer;
}

bool NativeFileProvider::exists(const std::string& path) {
    return std::filesystem::exists(path);
}

} // namespace ember::core