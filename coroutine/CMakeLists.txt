cmake_minimum_required(VERSION 2.6)
set(CMAKE_CXX_COMPILER "clang++")

project(coroutine)

set(LIBCO "co")
set(CMAKE_CXX_FLAGS "-ggdb -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -pg")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")

#find_package(Boost COMPONENTS thread system chrono)
include_directories(/opt/include)
include_directories(include)
list(APPEND lib_srcs src/coroutine.cc)

add_library(${LIBCO} SHARED ${lib_srcs})
add_subdirectory(examples)
