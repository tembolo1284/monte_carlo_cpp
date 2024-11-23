cmake_minimum_required(VERSION 3.10)
project(MonteCarloProject)

# Core settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Release)

# Compiler flags
add_compile_options(
    -O3
    -march=native
    -Wall
    -Wextra
    -Wpedantic
)

# Enable OpenMP
find_package(OpenMP REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Main executable
add_executable(MonteCarloProject
    main.cpp
    src/StopWatch.cpp
)

# Link OpenMP
if(OpenMP_CXX_FOUND)
    target_link_libraries(MonteCarloProject PRIVATE OpenMP::OpenMP_CXX)
endif()

# Testing setup
enable_testing()

# Download and build GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
)
FetchContent_MakeAvailable(googletest)

# Create test executable
add_executable(monte_carlo_tests
    tests/test_option_pricing.cpp
    tests/test_sde.cpp
    tests/test_fdm.cpp
)

target_link_libraries(monte_carlo_tests
    PRIVATE
    GTest::gtest
    GTest::gtest_main
    OpenMP::OpenMP_CXX
)

# Enable testing with GoogleTest
include(GoogleTest)
gtest_discover_tests(monte_carlo_tests)

# Print configuration
message(STATUS "")
message(STATUS "Build configuration:")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  OpenMP: ${OpenMP_CXX_FOUND}")
message(STATUS "")
