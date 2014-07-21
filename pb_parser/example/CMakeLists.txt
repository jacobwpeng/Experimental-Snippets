cmake_minimum_required(VERSION 2.6)

set(Lib "pbparser")
set(Program "run")

FIND_PACKAGE(Boost COMPONENTS REQUIRED system)
find_package(Protobuf REQUIRED)
find_library(PTHREAD "pthread")
find_library(BENCHMAKR "benchmark")
find_library(PBC "pbc")
find_library(GPREFTOOLS "profiler")
find_library(UNWIND "unwind")
include_directories(/opt/include)
include_directories(include)
include_directories(data/eager/)

list(APPEND ProgramSrcs main.cc ../data/eager/userinfo.pb.cc)

add_executable(${Program} ${ProgramSrcs})

target_link_libraries(${Program} ${PROTOBUF_LIBRARIES} ${Lib} ${PTHREAD} ${BENCHMAKR} ${PBC} ${GPREFTOOLS} ${UNWIND} ${Boost_LIBRARIES})
