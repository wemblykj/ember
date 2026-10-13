add_library(ember_graphics INTERFACE)

# Graphics sources
target_sources(ember_graphics PRIVATE
)

target_sources(ember_graphics PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
        graphics_types.h
        render_packet.h
        renderer.h
        surface_provider.h
 )

target_include_directories(ember_graphics INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ember_graphics INTERFACE
    ember_cxx_requirements
    ember_core
    ember_math
    ember_platform
    ember_resources
)
