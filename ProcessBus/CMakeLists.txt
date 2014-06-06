cmake_minimum_required(VERSION 2.6)
set(CMAKE_CXX_COMPILER "clang++")

project(processbus)

set(Lib "processbus")
set(Client "logclient")
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable")
set(CMAKE_CXX_FLAGS_RELEASE "-ggdb -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O3 -DNDEBUG")

find_package(Boost COMPONENTS thread system date_time)
find_library(RT rt)
find_library(PTHREAD pthread)
include_directories(/opt/include)
include_directories(include)

list(APPEND LibSrcs src/ring_buffer.cc src/process_bus.cc)
list(APPEND ClientSrcs src/client.cc)

add_library(${Lib} ${LibSrcs})

add_executable(${Client} ${ClientSrcs})
target_link_libraries(${Client} ${PTHREAD} ${RT} ${Boost_LIBRARIES} ${Lib})
