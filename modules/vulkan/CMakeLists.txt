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

    src/vulkan_instance.cpp
    src/scoped_vulkan_instance.cpp
    src/vulkan_renderer.cpp
    src/context/vulkan_context_vma.cpp
    src/context/any_physical_device_selector.cpp
    src/context/ranked_physical_device_selector.cpp
    src/context/default_physical_device_enumerator.cpp
    src/context/default_physical_device_selector.cpp
    src/pipeline/vulkan_pipeline_builder.cpp
    src/pipeline/shaderc_vulkan_shader_compiler.cpp 
    src/resources/vulkan_resource_cache.cpp
    src/resources/default_vulkan_resource_builder.cpp
    src/resources/default_vulkan_resource_file_provider.cpp
)

# Private headers
target_sources(ember_vulkan PRIVATE
    src/resources/vulkan_format_mapping.hpp
)

target_sources(ember_vulkan PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
        include/vulkan/vulkan_defs.hpp
        include/vulkan/vulkan_instance.hpp
        include/vulkan/scoped_vulkan_instance.hpp
        include/vulkan/vulkan_renderer.hpp
        include/vulkan/context/physical_device.hpp
        include/vulkan/context/vulkan_context.hpp
        include/vulkan/context/any_physical_device_selector.hpp
        include/vulkan/context/default_physical_device_selector.hpp
        include/vulkan/context/ranked_physical_device_selector.hpp
        include/vulkan/pipeline/vulkan_pipeline_builder.hpp
        include/vulkan/resources/vulkan_resource_builder.hpp
        include/vulkan/resources/vulkan_resource_defs.hpp
        include/vulkan/resources/default_vulkan_resource_file_provider.hpp
        include/vulkan/presentation/vulkan_surface_provider.hpp
)

target_include_directories(ember_vulkan 
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${Vulkan_INCLUDE_DIRS}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
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
    <core/ember_core.hpp>
    <math/ember_math.hpp>
    <resources/ember_resources.hpp>
    <platform/ember_platform.hpp>
    <graphics/ember_graphics.hpp>
)
