cmake_minimum_required(VERSION 2.6)
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")

project(LogFiles)

set(LIBLOGFILE "logfile")
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable")
set(CMAKE_CXX_FLAGS_RELEASE "-ggdb -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O3 -DNDEBUG")

#find_package(Boost COMPONENTS thread system chrono)
include_directories(/opt/include)
include_directories(include)

list(APPEND srcs src/log_file.cc src/log_fd_file.cc src/log_cfile.cc)

add_library(${LIBLOGFILE} ${srcs})
add_subdirectory(examples)
