#include <canned_resource_file_provider.h>

namespace ember::resources {

CannedResourceFileProvider::CannedResourceFileProvider() {
}

std::optional<std::string> CannedResourceFileProvider::readText(std::string_view path) const {
	return nullopt;
}

std::optional<std::vector<std::byte>> CannedResourceFileProvider::readBinary(std::string_view path) const {
	return nullopt;
}

bool CannedResourceFileProvider::exists(std::string_view path) const {
	return false;
}

} // namespace ember::resources
