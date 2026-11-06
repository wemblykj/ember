add_library(ember_platform INTERFACE)

target_sources(ember_platform INTERFACE
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
        include/platform/ember_platform.hpp
        include/platform/window.hpp
)

target_include_directories(ember_platform INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(ember_platform INTERFACE
    ember_cxx_requirements
    ember_core
)
