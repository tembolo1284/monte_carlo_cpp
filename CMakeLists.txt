cmake_minimum_required(VERSION 3.10)

# Set policy for INTERPROCEDURAL_OPTIMIZATION at the highest scope
if(POLICY CMP0069)
    cmake_policy(SET CMP0069 NEW)
endif()

# Add processor count for LTO jobs
include(ProcessorCount)
ProcessorCount(N)
if(N EQUAL 0)
    set(N 1)
endif()

project(MonteCarloProject)

# Force CMake to use our policy settings for FetchContent
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)

# Options
option(USE_CLANG "Use clang++ compiler" OFF)
option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

# Core settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Download and build GoogleTest BEFORE enabling LTO
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
)

# Force GoogleTest to use the same policies
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Get GoogleTest source and binary directories
FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    
    # Create policy override file for GoogleTest
    file(WRITE "${googletest_SOURCE_DIR}/policy_overrides.cmake"
        "if(POLICY CMP0069)\n  cmake_policy(SET CMP0069 NEW)\nendif()\n")
    
    # Include policy overrides
    set(CMAKE_MODULE_PATH "${googletest_SOURCE_DIR}" ${CMAKE_MODULE_PATH})
    include(policy_overrides)
    
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# Now enable LTO after GoogleTest is configured
include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)
if(supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    message(STATUS "IPO / LTO enabled with ${N} jobs")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # Set both compile and link flags for LTO with proper job count
        add_compile_options(-flto=${N})
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto=${N} -fuse-linker-plugin")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -flto=${N} -fuse-linker-plugin")
    endif()
    
    # Apply IPO to GoogleTest targets
    if(TARGET gtest)
        set_target_properties(gtest gtest_main gmock gmock_main
            PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
else()
    message(STATUS "IPO / LTO not supported: <${error}>")
endif()

# Compiler flags
add_compile_options(
    -O3
    -march=native
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Wcast-align
    -Wunused
    -Woverloaded-virtual
    -Wsign-conversion
    -Wnull-dereference
    -Wdouble-promotion
    -Wformat=2
    -Wimplicit-fallthrough
)

# GCC specific flags
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(
        -Wmisleading-indentation
        -Wduplicated-cond
        -Wduplicated-branches
        -Wlogical-op
        -Wuseless-cast
        -ffast-math
    )
endif()

# Coverage flags
if(ENABLE_COVERAGE AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options(--coverage)
        add_link_options(--coverage)
    endif()
endif()

# Enable OpenMP
find_package(OpenMP REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Main executable
add_executable(MonteCarloProject
    main.cpp
    src/StopWatch.cpp
)

# Set executable properties
set_target_properties(MonteCarloProject PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION TRUE
)

# Link OpenMP
if(OpenMP_CXX_FOUND)
    target_link_libraries(MonteCarloProject PRIVATE OpenMP::OpenMP_CXX)
endif()

# Testing setup
enable_testing()

# Create test executable
add_executable(monte_carlo_tests
    tests/test_option_pricing.cpp
    tests/test_sde.cpp
    tests/test_fdm.cpp
)

# Set test executable properties
set_target_properties(monte_carlo_tests PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION TRUE
)

target_link_libraries(monte_carlo_tests
    PRIVATE
    GTest::gtest
    GTest::gtest_main
    OpenMP::OpenMP_CXX
    pthread
)

# Enable testing with GoogleTest
include(GoogleTest)
gtest_discover_tests(monte_carlo_tests)

# Add custom targets
add_custom_target(run
    COMMAND MonteCarloProject
    DEPENDS MonteCarloProject
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running Monte Carlo simulation..."
)

add_custom_target(test_verbose
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose
    DEPENDS monte_carlo_tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running tests with verbose output..."
)

# Print configuration
message(STATUS "")
message(STATUS "Build configuration:")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  OpenMP: ${OpenMP_CXX_FOUND}")
message(STATUS "  LTO: ${supported}")
message(STATUS "  LTO Jobs: ${N}")
message(STATUS "  Coverage: ${ENABLE_COVERAGE}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "")
