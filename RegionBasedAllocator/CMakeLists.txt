cmake_minimum_required(VERSION 2.6)
project(prototype)
set(PROG "prog")
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -Werror -Wno-unused-variable -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb")
string(TOLOWER "${CMAKE_BUILD_TYPE}" lower_build_type)
set(PRECOMPILED_HEADER_PATH "${PROJECT_BINARY_DIR}/precompiled")
file(MAKE_DIRECTORY ${PRECOMPILED_HEADER_PATH})
if (lower_build_type STREQUAL "debug") 
    set(PRECOMPILED_DUMMY_HEADER "${PRECOMPILED_HEADER_PATH}/stdc++-debug.h")
    set(PRECOMPILED_HEADER "${PRECOMPILED_HEADER_PATH}/stdc++-debug.h.gch")
    separate_arguments(FLAGS UNIX_COMMAND "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
elseif (lower_build_type STREQUAL "release")
    set(PRECOMPILED_DUMMY_HEADER "${PRECOMPILED_HEADER_PATH}/stdc++-release.h")
    set(PRECOMPILED_HEADER "${PRECOMPILED_HEADER_PATH}/stdc++-release.h.gch")
    separate_arguments(FLAGS UNIX_COMMAND "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
else()
    set(PRECOMPILED_DUMMY_HEADER "${PRECOMPILED_HEADER_PATH}/stdc++.h")
    set(PRECOMPILED_HEADER "${PRECOMPILED_HEADER_PATH}/stdc++.h.gch")
    separate_arguments(FLAGS UNIX_COMMAND ${CMAKE_CXX_FLAGS})
endif()
list(APPEND SRCS "main.cc"
  "Page.cc"
  "LinkedList.cc"
  "PageAllocator.cc"
  "PageHeap.cc"
  "FreeList.cc"
  "Allocator.cc"
  )
find_path(STDC++-HEADER-PATH "stdc++.h" PATHS "/usr/include/c++/4.8.2/x86_64-redhat-linux/bits")
set(STDC++HEADER "${STDC++-HEADER-PATH}/stdc++.h")
set(PRECOMPILED_TARGET precompiled_header)
add_custom_target(${PRECOMPILED_TARGET}
    test -e ${PRECOMPILED_HEADER} || ${CMAKE_CXX_COMPILER} ${FLAGS} -x c++-header ${STDC++HEADER} -o ${PRECOMPILED_HEADER})
  #find_library(GLOG "glog")
add_executable(${PROG} ${SRCS})
set_target_properties(${PROG} PROPERTIES COMPILE_FLAGS "-include ${PRECOMPILED_DUMMY_HEADER}")
#target_link_libraries(${PROG} ${GLOG})
add_dependencies(${PROG} ${PRECOMPILED_TARGET})
