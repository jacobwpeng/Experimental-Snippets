cmake_minimum_required(VERSION 2.6)
set(CMAKE_CXX_COMPILER "clang++")

project(logsvrd)

set(Server "logsvrd")
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable")
set(CMAKE_CXX_FLAGS_RELEASE "-ggdb -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O3 -DNDEBUG")

find_package(Boost COMPONENTS thread system date_time)
find_library(RT rt)
find_library(PTHREAD pthread)
find_library(LOGFILE logfile /opt/lib)
find_library(ProcessBus processbus /opt/lib)
include_directories(/opt/include)
include_directories(include)

list(APPEND ServerSrcs src/main.cc src/logsvrd.cc)

add_executable(${Server} ${ServerSrcs})
target_link_libraries(${Server} ${PTHREAD} ${RT} ${Boost_LIBRARIES} ${LOGFILE} ${ProcessBus})
