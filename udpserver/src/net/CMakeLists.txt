CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

include_directories(${PROJECT_SOURCE_DIR}/base)

list(APPEND net_srcs channel.cc poller.cc event_loop.cc udp_listener.cc udp_server.cc)

add_library("fxnet" STATIC ${net_srcs})
target_link_libraries("fxnet" "fxbase")
