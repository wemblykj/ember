add_library(ember_core STATIC
    logger.cpp
    math.h
)

target_include_directories(ember_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(ember_core PRIVATE cxx_std_17)
