project(logfile_examples)

set(APP "run")
set(LIB "logfile")

find_package(Boost COMPONENTS REQUIRED system)
find_library(LIBBENCHMARK "benchmark" /opt/lib)
include_directories(/opt/include)
list(APPEND app_srcs main.cc)

add_executable(${APP} ${app_srcs})
target_link_libraries(${APP} ${Boost_LIBRARIES} ${LIBBENCHMARK} ${LIB})
