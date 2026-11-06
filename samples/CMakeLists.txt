cmake_minimum_required(VERSION 3.20)

add_executable(ember_sample
    sample_main.cpp
    sample_scene.cpp
)

target_include_directories(ember_sample PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ember_sample PRIVATE
    ember_core
    ember_platform
    ember_resources
    ember_graphics
    ember_application
)

target_compile_features(ember_sample PRIVATE cxx_std_20)

target_precompile_headers(ember_sample PRIVATE
    <string>
    <core/ember_core.hpp>
    <math/ember_math.hpp>
    <platform/ember_platform.hpp>
    <resources/ember_resources.hpp>
    <graphics/ember_graphics.hpp>
)
