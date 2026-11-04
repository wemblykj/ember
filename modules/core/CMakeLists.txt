add_library(ember_core STATIC
    src/logger.cpp
)

target_sources(ember_core PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
        include/core/ember_core.hpp
        include/core/logger.hpp
        include/core/scope_guard.hpp
        include/core/cleanup_stack.hpp
        include/core/graphics_types.hpp
        include/core/graphics_api.hpp
)

target_include_directories(ember_core 
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(ember_core PUBLIC 
    ember_cxx_requirements
)

target_precompile_headers(ember_core PRIVATE
    <string>
    <core/ember_core.hpp>
)
