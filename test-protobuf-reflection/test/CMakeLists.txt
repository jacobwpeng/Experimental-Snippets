CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(APP "test-protobuf-reflection")
SET(CMAKE_FIND_LIBRARY_PREFIXES "lib")
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".a")

INCLUDE_DIRECTORIES(/opt/include)
INCLUDE_DIRECTORIES(../include)
INCLUDE_DIRECTORIES(../../Singleton/include)

FIND_LIBRARY(PROTOBUF "protobuf")
FIND_LIBRARY(GTEST "gtest")
LIST(APPEND APP_SRC_LIST main.cc test.cc ../src/exchange_gifts_info.pb.cc ../src/exchange_gifts.cc ../src/exchange_gifts_info_mgr.cc)

ADD_DEFINITIONS( -O2 )

ADD_EXECUTABLE( ${APP} ${APP_SRC_LIST} )
TARGET_LINK_LIBRARIES( ${APP} ${PROTOBUF} ${GTEST})

ADD_TEST(test_reflection test-protobuf-reflection)
