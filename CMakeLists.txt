cmake_minimum_required(VERSION 3.20)
project(eEmber VERSION 0.1.0 LANGUAGES CXX)

option(EMBER_BUILD_SDL3 "Build using SDL3 support" ON)
option(EMBER_BUILD_SDL2 "Build using SDL2 support" OFF)
option(EMBER_BUILD_SAMPLES "Build the ember sample programs" ON)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build configuration
set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")

# Find required packages
find_package(Vulkan REQUIRED)

if (EMBER_BUILD_SDL3)
	find_package(SDL3 REQUIRED)
endif()

if (EMBER_BUILD_SDL2)
	find_package(SDL2 REQUIRED)
endif()


find_package(glm REQUIRED)

# Include directories for CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Add source subdirectories
add_subdirectory(src/core)
add_subdirectory(src/platform)
add_subdirectory(src/graphics)

if (EMBER_BUILD_SAMPLES)

	# Add samples
	add_subdirectory(samples)

	# default startup project for Visual Studio
	if (MSVC)
		set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT samples)
	endif()

endif()

if (BUILD_TESTING)

	# Enable testing
	enable_testing()

endif()

# Print configuration info
message(STATUS "=== Ember Engine Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Vulkan: ${Vulkan_FOUND}")

if (EMBER_BUILD_SDL3)
	message(STATUS "SDL3: ${SDL3_FOUND}")
endif()

if (EMBER_BUILD_SDL2)
	message(STATUS "SDL2: ${SDL2_FOUND}")
endif()

message(STATUS "GLM: ${glm_FOUND}")
message(STATUS "==================================")
