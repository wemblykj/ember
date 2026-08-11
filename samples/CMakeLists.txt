cmake_minimum_required(VERSION 3.20)

add_executable(ember_sample
    sample_main.cpp
    sample_scene.cpp
    ../src/application.cpp
)

target_include_directories(ember_sample PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

target_link_libraries(ember_sample PRIVATE
    ember_core
    ember_platform
    ember_graphics
)

target_compile_features(ember_sample PRIVATE cxx_std_17)
