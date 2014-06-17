cmake_minimum_required(VERSION 2.6)

set(Lib "pbparser")
set(Program "run")

find_package(Protobuf REQUIRED)
find_library(PTHREAD "pthread")
find_library(BENCHMAKR "benchmark")
find_library(PBC "pbc" /opt/lib)
include_directories(/opt/include)
include_directories(include)
include_directories(data)

list(APPEND ProgramSrcs main.cc)

add_executable(${Program} ${ProgramSrcs})

target_link_libraries(${Program} ${PROTOBUF_LIBRARIES} ${PBC} ${Lib} ${PTHREAD} ${BENCHMAKR})
