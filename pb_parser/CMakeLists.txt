cmake_minimum_required(VERSION 2.6)
set(CMAKE_CXX_COMPILER "clang++")

project(pb_parser)

set(Program "pbparser")
set(CMAKE_CXX_FLAGS "-ggdb -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -Wno-unused-variable")
set(CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS})
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")

#find_package(Boost COMPONENTS thread system date_time)
#find_library(RT rt)
#find_library(PTHREAD pthread)
find_package(Protobuf REQUIRED)
include_directories(/opt/include)
include_directories(include)

list(APPEND ProgramSrcs src/main.cc src/compact_protobuf.cc src/protobuf_parser.cc src/protobuf_helper.cc)

add_executable(${Program} ${ProgramSrcs})
target_link_libraries(${Program} ${PROTOBUF_LIBRARIES})#${PTHREAD} ${RT} ${Boost_LIBRARIES} ${LOGFILE} ${ProcessBus})
