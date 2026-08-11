add_library(ember_core STATIC
    logger.cpp
)

target_include_directories(ember_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# GLM for math
find_package(glm REQUIRED)
target_link_libraries(ember_core PUBLIC glm)

target_compile_features(ember_core PRIVATE cxx_std_17)
