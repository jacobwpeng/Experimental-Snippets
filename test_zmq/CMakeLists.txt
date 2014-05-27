cmake_minimum_required(VERSION 2.6)

set(APP "run")
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable")
set(CMAKE_CXX_FLAGS_RELEASE "-ggdb -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-const-variable -O3 -DNDEBUG")

find_package(Boost COMPONENTS REQUIRED system thread)
find_library(ZMQ "zmq")

include_directories(/opt/include)
list(APPEND srcs main.cc)

add_executable(${APP} ${srcs})
target_link_libraries(${APP} ${Boost_LIBRARIES} ${ZMQ})
