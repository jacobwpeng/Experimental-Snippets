cmake_minimum_required(VERSION 2.6)
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")

project(benchmark)

set(LIBBENCHMARK "benchmark")
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -Wno-unused-variable")
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

#find_package(Boost COMPONENTS thread system chrono)
include_directories(/opt/include include)

list(APPEND lib_srcs src/benchmark.cc)

add_library(${LIBBENCHMARK} STATIC ${lib_srcs})
add_subdirectory(examples)
