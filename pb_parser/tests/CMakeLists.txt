cmake_minimum_required(VERSION 2.6)

set(BIN "gtest-all")

include_directories(/opt/include)
include_directories(${PROJECT_SOURCE_DIR}/../include)

set(GTEST_ROOT "/usr/include/gtest")
message(STATUS ${GTEST_ROOT})
find_package(GTest REQUIRED) 
find_package(Protobuf REQUIRED)
find_library(PTHREAD "pthread" )
find_library(GLOG "glog")
list(APPEND SRCS gtest-all.cc)

add_executable( ${BIN} ${SRCS} )
target_link_libraries(${BIN} "pbparser" ${GTEST_LIBRARIES} ${PROTOBUF_LIBRARIES} ${PTHREAD} ${GLOG})
add_test("gtest-all" ${BIN})
