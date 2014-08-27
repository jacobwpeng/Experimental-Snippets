set (server "arenasvrd")
find_library(protobuf "protobuf")

include_directories(${PROJECT_SOURCE_DIR}/base)
include_directories(${PROJECT_SOURCE_DIR}/net)

list(APPEND arenasvrd_srcs arena_server.cc arena_conf.cc arena_message.pb.cc main.cc)

add_executable(${server} ${arenasvrd_srcs})
target_link_libraries(${server} "fxbase" "fxnet" ${GLOG} ${protobuf})
