set (server "arenasvrd")
find_library(protobuf "protobuf")
find_library(pthread "pthread")
find_library(gflags "gflags")

#set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
include_directories(${PROJECT_SOURCE_DIR}/base)
include_directories(${PROJECT_SOURCE_DIR}/net)

list(APPEND arenasvrd_srcs arena_server.cc arena_conf.cc arena_message.pb.cc main.cc)

add_executable(${server} ${arenasvrd_srcs})
target_link_libraries(${server} "fxbase" "fxnet" ${GLOG} ${protobuf} ${pthread} ${gflags})
