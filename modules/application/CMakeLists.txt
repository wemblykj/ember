add_library(ember_application STATIC
    "src/application.cpp"
    "src/native_file_provider.cpp"
)

target_sources(ember_application PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
        "include/application/ember_application.hpp"
        "include/application/application.hpp"
        "include/application/native_file_provider.hpp"
)

target_include_directories(ember_application PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(ember_application PUBLIC
    ember_cxx_requirements
    ember_core
    ember_platform
    ember_resources
    ember_graphics
    ember_vulkan
    ember_sdl3
)

target_precompile_headers(ember_application PRIVATE
    <string>
    <core/ember_core.hpp>
    <platform/ember_platform.hpp>
    <resources/ember_resources.hpp>
    <graphics/ember_graphics.hpp>
    <application/ember_application.hpp>
)
