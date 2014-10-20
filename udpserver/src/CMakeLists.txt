cmake_minimum_required(VERSION 2.6)
project(fx)

SET(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -Werror -Wno-unused-variable -ggdb" )
SET(CMAKE_CXX_FLAGS_DEBUG "")
SET(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3" )

include_directories(/opt/packages/glog/include)
find_library(GLOG "glog")

add_subdirectory(base)
add_subdirectory(net)
add_subdirectory(server)
#ADD_SUBDIRECTORY(examples)
#ENABLE_TESTING()
#ADD_SUBDIRECTORY(tests)
