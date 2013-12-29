CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT( test-protobuf-reflection )

SET(CMAKE_CXX_COMPILER "ccache-clang++")

SET(PROTO_DIR ${PROJECT_SOURCE_DIR}/proto)
SET(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
SET(SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
SET(TEST_SOURCE_DIR ${PROJECT_SOURCE_DIR}/test)

EXECUTE_PROCESS(COMMAND protoc -I=. --cpp_out=. exchange_gifts_info.proto WORKING_DIRECTORY ${PROTO_DIR} )
EXECUTE_PROCESS(COMMAND ln -f ${PROTO_DIR}/exchange_gifts_info.pb.h ${INCLUDE_DIR})
EXECUTE_PROCESS(COMMAND ln -f ${PROTO_DIR}/exchange_gifts_info.pb.cc ${SOURCE_DIR})

SET(EXTRA_FILES ${INCLUDE_DIR}/userinfo.pb.h ${SOURCE_DIR}/userinfo.pb.cc ${TEST_SOURCE_DIR}/userinfo.pb.cc)
#todo : remove all EXTRA_FILES ON make clean

ENABLE_TESTING()
ADD_SUBDIRECTORY( test test_bin )
