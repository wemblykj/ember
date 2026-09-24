if (EMBER_BUILD_SDL3)
    add_library(ember_platform STATIC
        sdl3/sdl3_window.cpp)

	find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)

    target_link_libraries(ember_platform PUBLIC SDL3::SDL3)

endif()

if (EMBER_BUILD_SDL2)
    add_library(ember_platform STATIC
        sdl2/sdl2_window.cpp
    )

	find_package(SDL2 REQUIRED)

    target_include_directories(ember_platform PUBLIC
        ${SDL2_INCLUDE_DIRS}
    )

    target_link_libraries(ember_platform PUBLIC
        ${SDL2_LIBRARIES}
    )

endif()

target_sources(ember_platform PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
        ember_platform.h
)

target_include_directories(ember_platform PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ember_platform PUBLIC
    ember_cxx_requirements
    ember_core
)

target_precompile_headers(ember_platform PRIVATE
    <string>
    <ember_core.h>
)
