cmake_minimum_required(VERSION 3.10)

project(MonteCarloProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Option for choosing the compiler: g++ or clang++
option(USE_CLANG "Use clang++ compiler" OFF)

if(USE_CLANG)
    set(CMAKE_CXX_COMPILER "clang++")
    message(STATUS "Using clang compiler")
else()
    set(CMAKE_CXX_COMPILER "g++")
    message(STATUS "Using g++ compiler")
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(MonteCarloProject
    main.cpp
    src/StopWatch.cpp
)

# cmake -DUSE_CLANG=ON -B build..
# cmake -DUSE_CLANG=OFF -B build..