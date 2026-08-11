add_library(ember_platform STATIC
    sdl2/sdl2_window.cpp
)

# Include SDL2
find_package(SDL2 REQUIRED)

target_include_directories(ember_platform PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${SDL2_INCLUDE_DIRS}
)

target_link_libraries(ember_platform PUBLIC
    ${SDL2_LIBRARIES}
    ember_core
)

target_compile_features(ember_platform PRIVATE cxx_std_17)
