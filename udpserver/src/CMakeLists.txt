cmake_minimum_required(VERSION 2.6)
project(fx)

SET(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wno-unused-variable -Wno-unused-private-field -ggdb" )
SET(CMAKE_CXX_FLAGS_DEBUG "")
SET(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3" )

find_library(GLOG "glog")
find_package(Boost COMPONENTS REQUIRED system thread)

add_subdirectory(base)
add_subdirectory(net)
add_subdirectory(server)
#ADD_SUBDIRECTORY(examples)
#ENABLE_TESTING()
#ADD_SUBDIRECTORY(tests)
