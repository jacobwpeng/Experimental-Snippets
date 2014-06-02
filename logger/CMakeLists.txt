cmake_minimum_required(VERSION 2.6)
set(CMAKE_CXX_COMPILER "clang++")

project(logger)

set(APP "run")
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable")
set(CMAKE_CXX_FLAGS_RELEASE "-ggdb -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O3 -DNDEBUG")

find_package(Boost COMPONENTS thread system chrono)
include_directories(/opt/include)
include_directories(include)
list(APPEND srcs src/main.cc 
                 src/log_file.cc 
                 src/log_streambuf.cc
                 src/log_stream.cc
                 src/log_formatter.cc
                 src/sync_logger.cc 
                 src/async_logger.cc
     )

add_executable(${APP} ${srcs})
target_link_libraries(${APP} ${Boost_LIBRARIES})
