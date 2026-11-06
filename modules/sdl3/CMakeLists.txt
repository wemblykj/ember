option(EMBER_BUILD_SDL3 "Build with SDL3 support" ON)

if (EMBER_BUILD_SDL3)
    add_library(ember_sdl3 STATIC
        src/platform/sdl3_window.cpp
        src/presentation/vulkan_surface_provider_sdl3.cpp
        )

	find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)

    target_link_libraries(ember_sdl3 PUBLIC SDL3::SDL3)

    target_sources(ember_sdl3 PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        FILES
            include/sdl3/ember_sdl3.hpp
            include/sdl3/platform/sdl3_window.hpp
            include/sdl3/presentation/vulkan_surface_provider_sdl3.hpp
    )

    target_include_directories(ember_platform INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )

    target_link_libraries(ember_sdl3 PUBLIC
        ember_cxx_requirements
        ember_core
        ember_platform
        ember_vulkan
    )

    target_precompile_headers(ember_sdl3 PRIVATE
        <string>
        <core/ember_core.hpp>
        <platform/ember_platform.hpp>
    )
endif()
