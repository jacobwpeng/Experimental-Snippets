CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT( FX_Framework )

SET(PROG "fx")
SET(CMAKE_FIND_LIBRARY_PREFIXES "lib")
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a" ".so")
SET(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Werror")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -Wall -Werror")
SET(CMAKE_LIBRARY_PATH "/opt/Judy/lib /usr/lib")

INCLUDE_DIRECTORIES(/opt/Judy/include)
INCLUDE_DIRECTORIES(/opt/boost/include)
INCLUDE_DIRECTORIES(include)

LIST(APPEND PROG_SRC src/main.cc)

FIND_LIBRARY(JUDY "Judy")

ADD_EXECUTABLE( ${PROG} ${PROG_SRC} ${JUDY})
TARGET_LINK_LIBRARIES( ${PROG} ${JUDY} )
