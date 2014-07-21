cmake_minimum_required(VERSION 2.6)
set(CMAKE_CXX_COMPILER "ccache-clang++")

project(pb_parser)

set(Lib "pbparser")
set(CMAKE_CXX_FLAGS "-ggdb -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -Wno-unused-variable")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -pg")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG -DBOOST_SP_NO_ATOMIC_ACCESS")

find_package(Protobuf REQUIRED)
#find_library(PTHREAD "pthread")
include_directories(/opt/include)
include_directories(include)
include_directories(data/eager/)

list(APPEND LibSrcs src/compact_protobuf.cc 
                    src/protobuf_parser.cc 
                    src/protobuf_helper.cc
                    src/protobuf_encoder.cc
                    )

add_library(${Lib} ${LibSrcs})

add_subdirectory(example)
add_subdirectory(tests)
