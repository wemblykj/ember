#include "native_file_provider.h"
#include <filesystem>
#include <fstream>

namespace ember::application {
	
NativeFileProvider::NativeFileProvider(const std::filesystem::path& root, bool enforceBoundary)
	: root_(root), enforceBoundary_(enforceBoundary) {
}

std::optional<std::string> NativeFileProvider::readText(std::string_view path) const {
    const auto fsPath = resolvePath(path);

    std::ifstream stream(fsPath, std::ios::in | std::ios::binary);
    if (!stream) {
        return std::nullopt;
    }

    std::string contents(
        (std::istreambuf_iterator<char>(stream)),
        std::istreambuf_iterator<char>());
    return contents;
}

std::optional<std::vector<std::byte>> NativeFileProvider::readBinary(std::string_view path) const {
	const auto fsPath = resolvePath(path);

    std::ifstream stream(fsPath, std::ios::in | std::ios::binary | std::ios::ate);
    if (!stream) {
        return std::nullopt;
    }

    const auto size = static_cast<std::size_t>(stream.tellg());
    stream.seekg(0, std::ios::beg);

    std::vector<std::byte> buffer(size);
    stream.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(size));
    return buffer;
}

bool NativeFileProvider::exists(std::string_view path) const {
    const auto fsPath = resolvePath(path);

    return std::filesystem::exists(fsPath);
}

std::filesystem::path NativeFileProvider::resolvePath(std::string_view path) const
{
    std::filesystem::path resolvedPath(root_);
    resolvedPath.append(path);

    if (enforceBoundary_) {
        std::filesystem::path canonicalRoot = std::filesystem::canonical(root_);
        std::filesystem::path canonicalResolved = std::filesystem::weakly_canonical(resolvedPath);
        if (!canonicalResolved.string().starts_with(canonicalRoot.string())) {
            throw std::runtime_error("Access to path outside of root directory is not allowed.");
        }
	}

    return resolvedPath;
}

} // namespace ember::application