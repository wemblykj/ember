add_library(ember_application STATIC
    application.cpp
    native_file_provider.cpp
)

target_sources(ember_application PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
        application.h
)

target_include_directories(ember_application PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
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
    <ember_core.h>
    <ember_platform.h>
    <ember_resources.h>
    <ember_graphics.h>
)
