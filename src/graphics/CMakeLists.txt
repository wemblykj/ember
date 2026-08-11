add_library(ember_graphics STATIC)

# Vulkan
find_package(Vulkan REQUIRED)

# Graphics sources
target_sources(ember_graphics PRIVATE
    vulkan/vulkan_context.cpp
    vulkan/vulkan_renderer.cpp
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

target_compile_features(ember_graphics PRIVATE cxx_std_17)
