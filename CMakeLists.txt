cmake_minimum_required(VERSION 3.20)
project(Ember VERSION 0.1.0 LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build configuration
set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")

# Include directories for CMake modules (must be before find_package if you ship Find*.cmake)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Find required packages
find_package(Vulkan REQUIRED)
find_package(SDL3 REQUIRED)
find_package(glm REQUIRED)

# Add source subdirectories
add_subdirectory(src/core)
add_subdirectory(src/platform)
add_subdirectory(src/graphics)

# Add samples
add_subdirectory(samples)

# Enable testing
enable_testing()

# central interface target for compile requirements
add_library(ember_cxx_requirements INTERFACE)
target_compile_features(ember_cxx_requirements INTERFACE cxx_std_20)
# optional: enforce required/disable extensions for consumers
target_compile_options(ember_cxx_requirements INTERFACE
    $<$<CXX_COMPILER_ID:MSVC>:/permissive->  # example MSVC option
)

# Print configuration info
message(STATUS "=== Ember Engine Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Vulkan: ${Vulkan_FOUND}")
message(STATUS "SDL3: ${SDL3_FOUND}")
message(STATUS "GLM: ${glm_FOUND}")
message(STATUS "==================================")
