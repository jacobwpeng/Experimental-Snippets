CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(BIN "trie")

INCLUDE_DIRECTORIES(/opt/include)
INCLUDE_DIRECTORIES(../../include)

SET(CMAKE_CXX_FLAGS "-ggdb -Wall -Wextra -Werror -Wno-invalid-offsetof -Wno-unused-parameter" )

#FIND_LIBRARY(GLOG "glog")
FIND_PACKAGE(Boost COMPONENTS REQUIRED system thread)
FIND_LIBRARY(PTHREAD "pthread")
LIST(APPEND SRC_LIST main.cc)

ADD_EXECUTABLE( ${BIN} ${SRC_LIST} )
TARGET_LINK_LIBRARIES( ${BIN} ${PTHREAD}
#                        ${GLOG} 
                        ${Boost_LIBRARIES}
                        )
