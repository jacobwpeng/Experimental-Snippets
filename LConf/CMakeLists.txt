CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT( LConf )

SET(LIB "lconf")
SET(LIBSO "lconf")
SET(APP "test")
SET(CMAKE_FIND_LIBRARY_PREFIXES "lib")
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".a")

INCLUDE_DIRECTORIES(/opt/include)
INCLUDE_DIRECTORIES(include)

FIND_LIBRARY(LUA "lua")
LIST(APPEND LIB_SRC_LIST src/lconf.cc src/table_node.cc src/scalar_node.cc)
LIST(APPEND APP_SRC_LIST src/test.cc)

ADD_DEFINITIONS( -std=c++11 )

ADD_LIBRARY( ${LIBSO} SHARED ${LIB_SRC_LIST} )

ADD_EXECUTABLE( ${APP} ${APP_SRC_LIST} )
TARGET_LINK_LIBRARIES( ${APP} ${LUA} ${LIBSO} )
