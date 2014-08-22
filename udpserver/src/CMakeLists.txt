CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(fx)

SET(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -Wno-unused-variable -Wno-unused-private-field -ggdb" )
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS}")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -DNDEBUG -O3" )

FIND_LIBRARY(GLOG "glog")
FIND_PACKAGE(Boost COMPONENTS REQUIRED system thread)

add_subdirectory(base)
add_subdirectory(net)
add_subdirectory(server EXCLUDE_FROM_ALL)
#ADD_SUBDIRECTORY(examples)
#ENABLE_TESTING()
#ADD_SUBDIRECTORY(tests)
