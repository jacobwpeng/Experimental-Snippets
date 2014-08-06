cmake_minimum_required(VERSION 2.6)
set(CMAKE_CXX_COMPILER "clang++")

set(APP "test-coroutine")
project(coroutine-example)

list(APPEND example-srcs main.cc)

add_executable(${APP} ${example-srcs})
target_link_libraries(${APP} ${LIBCO})
#target_link_libraries(${APP} ${Boost_LIBRARIES})
