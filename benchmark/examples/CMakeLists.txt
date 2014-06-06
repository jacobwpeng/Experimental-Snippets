project(benchmark_examples)

set(APP "run")

list(APPEND app_srcs main.cc)

add_executable(${APP} ${app_srcs})
target_link_libraries(${APP} "benchmark")
