CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(BIN "rbtree")

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})

SET(CMAKE_CXX_FLAGS "-std=c++11 -ggdb -Wall -Wextra -Werror -Wno-invalid-offsetof -Wno-unused-parameter" )

LIST(APPEND SRC_LIST main.cc)

ADD_EXECUTABLE( ${BIN} ${SRC_LIST} )
