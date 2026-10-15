add_library(ember_vulkan STATIC)

option(EMBER_BUILD_SHADERC_UNOFFICIAL "Build with unofficial shaderc support" ON)


# Vulkan
if (NOT EMBER_BUILD_SHADERC_UNOFFICIAL)

    find_package(Vulkan REQUIRED COMPONENTS shaderc_combined)

    target_link_libraries(ember_vulkan PRIVATE
        Vulkan::shaderc_combined
    )

else()

    find_package(Vulkan REQUIRED COMPONENTS)
    find_package(glslang CONFIG REQUIRED)
    find_package(SPIRV-Tools CONFIG REQUIRED)
    find_package(unofficial-shaderc CONFIG REQUIRED)

    target_link_libraries(ember_vulkan PRIVATE
        glslang::glslang glslang::glslang-default-resource-limits glslang::SPIRV
        SPIRV-Tools-static
        unofficial::shaderc::shaderc
    )

endif()

find_package(VulkanMemoryAllocator CONFIG REQUIRED)

# Graphics sources
target_sources(ember_vulkan PRIVATE
    context/vulkan_instance.cpp
    context/scoped_vulkan_instance.cpp
    context/vulkan_context_vma.cpp
    context/any_physical_device_selector.cpp
    context/ranked_physical_device_selector.cpp
    context/default_physical_device_selector.cpp
    resources/vulkan_resource_cache.cpp
    resources/default_vulkan_resource_builder.cpp
    resources/shaderc_vulkan_shader_compiler.cpp
    vulkan_renderer.cpp
)

target_sources(ember_vulkan PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
        vulkan_defs.h
        context/physical_device.h
        context/vulkan_instance.h
        context/scoped_vulkan_instance.h
        context/vulkan_context.h
        context/any_physical_device_selector.h
        context/default_physical_device_selector.h
        context/ranked_physical_device_selector.h
        presentation/vulkan_surface_provider.h
        resources/vulkan_resource_builder.h
        resources/vulkan_resource_defs.h
        vulkan_renderer.h
 )

target_include_directories(ember_vulkan PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(ember_vulkan PRIVATE
    Vulkan::Vulkan
    GPUOpen::VulkanMemoryAllocator
)

target_link_libraries(ember_vulkan PUBLIC
    ember_cxx_requirements
    ember_core
    ember_platform
    ember_resources
    ember_graphics
)

target_precompile_headers(ember_vulkan PRIVATE
    <string>
    <ember_core.h>
    <ember_math.h>
    <ember_platform.h>
    <ember_graphics.h>
)
