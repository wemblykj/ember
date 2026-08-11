cmake_minimum_required(VERSION 3.20)
project(Ember VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build configuration
set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")

# Find required packages
find_package(Vulkan REQUIRED)
find_package(SDL2 REQUIRED)
find_package(glm REQUIRED)

# Include directories for CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Add source subdirectories
add_subdirectory(src/core)
add_subdirectory(src/platform)
add_subdirectory(src/graphics)

# Add samples
add_subdirectory(samples)

# Enable testing
enable_testing()

# Print configuration info
message(STATUS "=== Ember Engine Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Vulkan: ${Vulkan_FOUND}")
message(STATUS "SDL2: ${SDL2_FOUND}")
message(STATUS "GLM: ${glm_FOUND}")
message(STATUS "==================================")
