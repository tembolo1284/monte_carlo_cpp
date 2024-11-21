cmake_minimum_required(VERSION 3.10)
project(MonteCarloProject)

# Set compiler options before project declaration
option(USE_CLANG "Use clang++ compiler" OFF)

# Set compiler path only if not already set
if(NOT CMAKE_CXX_COMPILER)
    if(USE_CLANG)
        find_program(CMAKE_CXX_COMPILER "clang++" REQUIRED)
    else()
        find_program(CMAKE_CXX_COMPILER "g++" REQUIRED)
    endif()
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add optimization flags
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto -ffast-math")
set(CMAKE_BUILD_TYPE Release)

# Coverage option
option(ENABLE_COVERAGE "Enable coverage reporting" OFF)
if(ENABLE_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif()

# Enable OpenMP
find_package(OpenMP REQUIRED)
if(OpenMP_CXX_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Main executable
add_executable(MonteCarloProject
    main.cpp
    src/StopWatch.cpp
)

if(OpenMP_CXX_FOUND)
    target_link_libraries(MonteCarloProject PRIVATE OpenMP::OpenMP_CXX)
endif()

# Testing setup
enable_testing()

# Configure External Project for GoogleTest
include(ExternalProject)
ExternalProject_Add(googletest
    GIT_REPOSITORY    https://github.com/google/googletest.git
    GIT_TAG           release-1.12.1
    SOURCE_DIR        "${CMAKE_BINARY_DIR}/googletest-src"
    BINARY_DIR        "${CMAKE_BINARY_DIR}/googletest-build"
    INSTALL_COMMAND   ""
)

# Create interface library for GoogleTest
add_library(GTest::GTest INTERFACE IMPORTED)
ExternalProject_Get_Property(googletest source_dir binary_dir)
set_property(TARGET GTest::GTest PROPERTY
            INTERFACE_INCLUDE_DIRECTORIES ${source_dir}/googletest/include)
set_property(TARGET GTest::GTest PROPERTY
            INTERFACE_LINK_LIBRARIES ${binary_dir}/lib/libgtest.a)
add_dependencies(GTest::GTest googletest)

# Create interface library for GoogleTest Main
add_library(GTest::Main INTERFACE IMPORTED)
set_property(TARGET GTest::Main PROPERTY
            INTERFACE_LINK_LIBRARIES ${binary_dir}/lib/libgtest_main.a)
add_dependencies(GTest::Main googletest)

# Create test executable
add_executable(monte_carlo_tests
    tests/test_option_pricing.cpp
    tests/test_sde.cpp
    tests/test_fdm.cpp
)

add_dependencies(monte_carlo_tests googletest)

target_link_libraries(monte_carlo_tests
    PRIVATE
    GTest::GTest
    GTest::Main
    OpenMP::OpenMP_CXX
    pthread
)

target_include_directories(monte_carlo_tests
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# Enable testing with GoogleTest
include(GoogleTest)
gtest_discover_tests(monte_carlo_tests)
