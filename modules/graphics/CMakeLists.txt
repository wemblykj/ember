add_library(ember_graphics INTERFACE)

# Graphics sources
target_sources(ember_graphics PRIVATE
)

target_sources(ember_graphics INTERFACE
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
        "include/graphics/ember_graphics.hpp"
        "include/graphics/renderer.hpp"
        "include/graphics/pipeline/render_packet.hpp"
        "include/graphics/presentation/surface_provider.hpp"
 )

target_include_directories(ember_graphics INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(ember_graphics INTERFACE
    ember_cxx_requirements
    ember_core
    ember_math
    ember_platform
    ember_resources
)
