add_library(ember_resources STATIC
    default_canned_resource_provider.cpp
)

target_sources(ember_resources PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
        ember_resources.h
        resource_types.h
        canned_resources.h
        resource_descriptors.h
        resource_file_provider.h
        resource_cache.h
        canned_resource_provider.h
        default_canned_resource_provider.h
)

target_include_directories(ember_resources PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ember_resources PUBLIC
    ember_cxx_requirements
    ember_core
)
