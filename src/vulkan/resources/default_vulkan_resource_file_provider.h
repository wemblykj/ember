#pragma once

#include <string_view>
#include <map>

#include <dictionary_based_file_provider.h>

namespace ember::vulkan::resources {

/**
 * @brief Serves up default Vulkan resource files.
 */
class DefaultVulkanResourceFileProvider : public ember::resources::DictionaryBasedFileProvider<std::string_view> {
public:
	DefaultVulkanResourceFileProvider();

private:
	std::map<std::string, std::string_view> createResources();
};

} // namespace ember::vulkan::resources