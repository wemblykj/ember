add_library(ember_resources STATIC
    src/default_canned_resource_provider.cpp
    src/dictionary_based_file_provider.cpp
    src/canned_resource_file_provider.cpp
)

target_sources(ember_resources PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
        include/resources/ember_resources.hpp
        include/resources/resource_types.hpp
        include/resources/canned_resources.hpp
        include/resources/resource_descriptors.hpp
        include/resources/resource_file_provider.hpp
        include/resources/resource_cache.hpp
        include/resources/canned_resource_provider.hpp
        include/resources/dictionary_based_file_provider.hpp
        include/resources/default_canned_resource_provider.hpp
)

target_include_directories(ember_resources 
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(ember_resources PUBLIC
    ember_cxx_requirements
    ember_core
)
