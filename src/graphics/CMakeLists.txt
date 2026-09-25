add_library(ember_graphics STATIC)

# Vulkan
find_package(Vulkan REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)

# Graphics sources
target_sources(ember_graphics PRIVATE
    vulkan/vulkan_context_vma.cpp
    vulkan/vulkan_resource_cache.cpp
    vulkan/vulkan_renderer.cpp
    vulkan/vulkan_surface_provider_sdl3.cpp
    vulkan/any_physical_device_selector.cpp
    vulkan/ranked_physical_device_selector.cpp
    vulkan/default_physical_device_selector.cpp
)

target_sources(ember_graphics PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
        graphics_types.h
        resources.h
        render_packet.h
        renderer.h
        surface_provider.h
        vulkan/vulkan_context.h
        vulkan/vulkan_renderer.h
        vulkan/vulkan_surface_provider.h
        vulkan/vulkan_surface_provider_sdl3.h
        vulkan/any_physical_device_selector.h
        vulkan/default_physical_device_selector.h
        vulkan/ranked_physical_device_selector.h
)

target_include_directories(ember_graphics PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(ember_graphics PRIVATE
    Vulkan::Vulkan
    GPUOpen::VulkanMemoryAllocator
)

target_link_libraries(ember_graphics PUBLIC
    Vulkan::Headers
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
