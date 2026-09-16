add_library(ember_graphics STATIC)

# Vulkan
find_package(Vulkan REQUIRED)

# Graphics sources
target_sources(ember_graphics PRIVATE
    #vulkan/vulkan_context.cpp
    vulkan/vulkan_renderer.cpp
)

target_sources(ember_graphics PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
        graphics_types.h
        resources.h
        render_packet.h
        renderer.h
)

target_include_directories(ember_graphics PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(ember_graphics PUBLIC
    ${Vulkan_LIBRARIES}
    ember_core
    ember_platform
)

target_link_libraries(ember_core PUBLIC ember_cxx_requirements)

target_precompile_headers(ember_graphics PRIVATE
    <string>
    <ember_core.h>
    <ember_math.h>
    <ember_platform.h>
)
