#include <canned_resource_file_provider.h>

namespace ember::resources {

CannedResourceFileProvider::CannedResourceFileProvider() 
	: DictionaryBasedFileProvider("shaders/canned/", createResources()) {
}

std::map<std::string, std::string_view> CannedResourceFileProvider::createResources() {
    std::map<std::string, std::string_view> resources{
		{
			"opaque.vert",
			""
		},
		{
			"opaque.frag",
			""
		},
		{
			"transparent.vert",
			""
		},
		{
			"transparent.frag",
			""
		},
    };

    // Add your canned resources here
    return resources;
}

} // namespace ember::resources
