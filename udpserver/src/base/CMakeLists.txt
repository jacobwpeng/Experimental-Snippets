CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(fxbase)

list(APPEND base_srcs system_time.cc)

add_library("fxbase" STATIC ${base_srcs})
