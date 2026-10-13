add_library(ember_resources INTERFACE)

target_sources(ember_resources INTERFACE
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
        resources.h
        resource_file_provider.h
        resource_cache.h
)

target_include_directories(ember_resources INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ember_resources INTERFACE
    ember_cxx_requirements
    ember_core
)
