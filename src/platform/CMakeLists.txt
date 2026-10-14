add_library(ember_platform INTERFACE)

target_sources(ember_platform INTERFACE
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
        ember_platform.h
        window.h
)

target_include_directories(ember_platform INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ember_platform INTERFACE
    ember_cxx_requirements
    ember_core
)
