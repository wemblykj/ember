add_library(ember_core STATIC
    logger.cpp
)

target_sources(ember_core PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
        ember_core.h
        logger.h
        graphics_api.h
)

target_include_directories(ember_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

find_package(glm REQUIRED)

target_compile_features(ember_core PRIVATE cxx_std_17)

target_precompile_headers(ember_core PRIVATE
    <string>
    "ember_core.h"
)
