#include "default_vulkan_resource_file_provider.h"

namespace {

const char* generic_transform = R"tag(
#version 450
layout(location = 0) in vec3 inPosition;
layout(location = 0) out vec3 outColor;

void main() {
    gl_Position = vec4(inPosition, 1.0);
    outColor = vec3(1.0, 0.0, 0.0); // red
}
)tag";

const char* solid_white = R"tag(
#version 450
layout(location = 0) in vec3 inColor;
layout(location = 0) out vec4 outFragColor;

void main() {
    outFragColor = vec4(inColor, 1.0);
}
)tag";

}

namespace ember::vulkan::resources {

DefaultVulkanResourceFileProvider::DefaultVulkanResourceFileProvider() 
	: DictionaryBasedFileProvider("shaders/canned/", createResources()) {
}

std::map<std::string, std::string_view> DefaultVulkanResourceFileProvider::createResources() {
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

} // namespace ember::vulkan::resources
