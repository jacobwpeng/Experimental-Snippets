cmake_minimum_required(VERSION 2.6)

set(Program "gtest-all")

include_directories(/opt/include)
include_directories(include)

#set(GTEST_ROOT "/usr/include/gtest")
find_package(GTest REQUIRED) 
find_package(Protobuf REQUIRED)
list(APPEND SRCS gtest-all.cc unittest_environment.cc
                              unittest_message.cc
                              )
add_executable( ${Program} ${SRCS} )
target_link_libraries(${Program} "pbparser" ${GTEST_LIBRARIES} ${PROTOBUF_LIBRARIES})
add_test("gtest-all" ${Program})
