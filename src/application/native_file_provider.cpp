#include "native_file_provider.h"
#include <filesystem>
#include <fstream>

namespace ember::application {
	
NativeFileProvider::NativeFileProvider(const std::filesystem::path& root, bool enforceBoundary)
	: root_(root), enforceBoundary_(enforceBoundary) {
}

std::unique_ptr<std::istream> NativeFileProvider::openText(std::string_view path) const {
	const auto fsPath = resolvePath(path);

    auto stream = std::make_unique<std::ifstream>(fsPath, std::ios::in);
    if (!*stream) {
		return std::make_unique<std::istringstream>(""); // Return an empty stream if the file cannot be opened
    }

    return stream;
}

std::unique_ptr<std::istream> NativeFileProvider::openBinary(std::string_view path) const {
	const auto fsPath = resolvePath(path);

    auto stream = std::make_unique<std::ifstream>(fsPath, std::ios::in | std::ios::binary);
    if (!*stream) {
		return std::make_unique<std::istringstream>(""); // Return an empty stream if the file cannot be opened
    }

    return stream;
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