#pragma once

#include <string_view>
#include <map>

#include <dictionary_based_file_provider.h>

namespace ember::resources {

/**
 * @brief Serves up canned resource files.
 */
class CannedResourceFileProvider : public DictionaryBasedFileProvider<std::string_view> {
public:
	CannedResourceFileProvider();

private:
	std::map<std::string, std::string_view> createResources();
};

} // namespace ember::resources