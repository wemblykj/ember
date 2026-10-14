option(EMBER_BUILD_SDL3 "Build with SDL3 support" ON)

if (EMBER_BUILD_SDL3)
    add_library(ember_sdl3 STATIC
        platform/sdl3_window.cpp
        presentation/vulkan_surface_provider_sdl3.cpp
        )

	find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)

    target_link_libraries(ember_sdl3 PUBLIC SDL3::SDL3)

    target_sources(ember_sdl3 PRIVATE
        platform/sdl3_window.h
        presentation/vulkan_surface_provider_sdl3.h
    )
    target_sources(ember_sdl3 PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        FILES
            ember_sdl3.h
            platform/sdl3_window.h
    )

    target_include_directories(ember_sdl3 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

    target_link_libraries(ember_sdl3 PUBLIC
        ember_cxx_requirements
        ember_core
        ember_platform
        ember_vulkan
    )

    target_precompile_headers(ember_sdl3 PRIVATE
        <string>
        <ember_core.h>
        <ember_platform.h>
    )
endif()
