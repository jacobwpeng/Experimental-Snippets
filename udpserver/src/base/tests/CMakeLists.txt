cmake_minimum_required(VERSION 2.6)

set(bin "gtest-all")
find_library(pthread "pthread")

include_directories(${PROJECT_SOURCE_DIR}/base)
include_directories(/opt/packages/gtest/include)

find_library(gtest "gtest" PATHS /opt/packages/gtest/lib)
list(APPEND base_test_srcs
            gtest-all.cc
            memory_list_unittest.cc
            list_unittest.cc
            rbtree_unittest.cc
    )

add_executable( ${bin} ${base_test_srcs} )
set_target_properties(${bin} PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-compare")
target_link_libraries( ${bin} "fxbase" ${pthread} ${gtest})
add_test("gtest-all" ${bin} )
