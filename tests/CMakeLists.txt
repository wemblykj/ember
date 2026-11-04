cmake_minimum_required(VERSION 3.24)
project(EmberTests LANGUAGES CXX)

# Enforce your C++20 standard requirements
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# 1. Automatically fetch Catch2 v3 from official sources
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.8.1 # Lock to a stable C++20 validated release
)
FetchContent_MakeAvailable(Catch2)

# 2. Declare your unit test executable
add_executable(token_tests 
    test_main.cpp
    test_token_registry.cpp
)

# 3. Link against Catch2's built-in main runner target
target_link_libraries(token_tests PRIVATE Catch2::Catch2WithMain)
target_include_directories(token_tests PRIVATE ../include)

# 4. Critical line: Registers individual TEST_CASEs directly with Visual Studio Test Explorer
include(CTest)
include(Catch)
catch_discover_tests(token_tests)