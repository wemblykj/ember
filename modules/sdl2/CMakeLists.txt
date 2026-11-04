option(EMBER_BUILD_SDL2 "Build with SDL2 support" OFF)

if (EMBER_BUILD_SDL2)
    add_library(ember_sdl2 STATIC
        platform/sdl2_window.cpp
    )

	find_package(SDL2 REQUIRED)

    target_include_directories(ember_sdl2 PUBLIC
        ${SDL2_INCLUDE_DIRS}
    )

    target_link_libraries(ember_sdl2 PUBLIC
        ${SDL2_LIBRARIES}
    )

    target_sources(ember_sdl2 PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
        ember_sdl2.h
        platform/sdl2_window.h
    )

    target_include_directories(ember_sdl2 PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

    target_link_libraries(ember_sdl2 PUBLIC
        ember_cxx_requirements
        ember_core
        ember_platform
    )

    target_precompile_headers(ember_sdl2 PRIVATE
        <string>
        <ember_core.h>
        <ember_platform.h>
    )
endif()


