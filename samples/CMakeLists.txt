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
    <ember_core.h>
    <ember_math.h>
    <ember_platform.h>
    <ember_resources.h>
    <ember_graphics.h>
)
