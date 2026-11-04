add_library(ember_math INTERFACE)

target_sources(ember_math INTERFACE
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
        ember_math.h
        math_types.h
)

find_package(glm REQUIRED)

target_include_directories(ember_math INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GLM_INCLUDE_DIRS}
)

target_link_libraries(ember_math INTERFACE
    ember_cxx_requirements
    ember_core
)

