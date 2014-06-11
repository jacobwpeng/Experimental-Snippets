project(benchmark_examples)

set(APP "run")

list(APPEND app_srcs main.cc userinfo.pb.cc)
find_library(PROTOBUF "protobuf")

add_executable(${APP} ${app_srcs})
target_link_libraries(${APP} "benchmark" ${PROTOBUF})
