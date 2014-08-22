CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(arenaserver)

include_directories(../net)

list(APPEND arena_srcs arena_server.cc main.cc)

add_executable("arenasvrd" ${arena_srcs})
target_link_libraries("arenasvrd" "fxnet" ${GLOG})
