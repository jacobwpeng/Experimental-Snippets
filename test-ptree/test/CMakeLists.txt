CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(APP "test-ptree")
SET(CMAKE_FIND_LIBRARY_PREFIXES "lib")
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".a")

INCLUDE_DIRECTORIES(/opt/include)
INCLUDE_DIRECTORIES(../include)

FIND_LIBRARY(GTEST "gtest")
LIST(APPEND APP_SRC_LIST main.cc test.cc )

ADD_DEFINITIONS( -O2 -DCONF_PATH="${PROJECT_SOURCE_DIR}/conf")
MESSAGE(STATUS "${PROJECT_SOURCE_DIR}/conf")

ADD_EXECUTABLE( ${APP} ${APP_SRC_LIST} )
TARGET_LINK_LIBRARIES( ${APP} ${GTEST})

ADD_TEST("ptree" ${APP})
