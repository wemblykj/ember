add_library(ember_graphics STATIC)

# Vulkan
find_package(Vulkan REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)

# Graphics sources
target_sources(ember_graphics PRIVATE
    vulkan/vulkan_context_vma.cpp
    vulkan/vulkan_resource_cache.cpp
    vulkan/vulkan_renderer.cpp
    vulkan/sdl3_vulkan_surface_provider.cpp
)

target_sources(ember_graphics PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
        graphics_types.h
        resources.h
        render_packet.h
        renderer.h
        vulkan/vulkan_surface_provider.h
        vulkan/sdl3_vulkan_surface_provider.h
)

target_include_directories(ember_graphics PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(ember_graphics PRIVATE
    Vulkan::Headers
    GPUOpen::VulkanMemoryAllocator
)

target_link_libraries(ember_graphics PUBLIC
    ember_cxx_requirements
    ember_core
    ember_platform
)

target_precompile_headers(ember_graphics PRIVATE
    <string>
    <ember_core.h>
    <ember_math.h>
    <ember_platform.h>
)
