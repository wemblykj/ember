#include "canned_resource_file_provider.h"

namespace {

const char* generic_transform = R"tag(
#version 330 core
layout(location = 0) in vec3 aPos;

void main() {
    gl_Position = vec4(aPos, 1.0);
})tag";

const char* solid_white = R"tag(
#version 330 core
out vec4 FragColor;

void main() {
    FragColor = vec4(1.0); // solid white
}
)tag";

}

namespace ember::resources {

CannedResourceFileProvider::CannedResourceFileProvider() 
	: DictionaryBasedFileProvider("shaders/canned/", createResources()) {
}

std::map<std::string, std::string_view> CannedResourceFileProvider::createResources() {
    std::map<std::string, std::string_view> resources{
		{
			"invalid.vert",
			generic_transform
		},
		{
			"undefined.vert",
			generic_transform
		},
		{
			"invalid.frag",
			solid_white
		},
		{
			"undefined.frag",
			solid_white
		},
		{
			"generic_transform.vert",
			generic_transform
		},
		{
			"opaque_pbr.frag",
			""
		},
		{
			"transparent_pbr.frag",
			""
		},
    };

    // Add your canned resources here
    return resources;
}

} // namespace ember::resources
