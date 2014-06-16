cmake_minimum_required(VERSION 2.6)
set(CMAKE_CXX_COMPILER "ccache-clang++")

project(pb_parser)

set(Lib "pbparser")
#set(Program "run")
set(CMAKE_CXX_FLAGS "-ggdb -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -Wno-unused-variable")
set(CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS})
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")

find_package(Protobuf REQUIRED)
find_library(PTHREAD "pthread")
find_library(BENCHMAKR "benchmark")
find_library(PBC "pbc" /opt/lib)
include_directories(/opt/include)
include_directories(include)
include_directories(data)

#list(APPEND ProgramSrcs src/main.cc)
list(APPEND LibSrcs src/compact_protobuf.cc 
                    src/protobuf_parser.cc 
                    src/protobuf_helper.cc
                    src/protobuf_encoder.cc
                    data/userinfo.pb.cc
                    )

add_library(${Lib} ${LibSrcs})
#add_executable(${Program} ${ProgramSrcs})

#target_link_libraries(${Program} ${PROTOBUF_LIBRARIES} ${Lib} ${PTHREAD} ${BENCHMAKR} ${PBC})

add_subdirectory(example)
add_subdirectory(tests)
