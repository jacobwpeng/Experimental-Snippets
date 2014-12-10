CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

list(APPEND base_srcs system_time.cc memory_list.cc mmap_file.cc)

add_library("fxbase" STATIC ${base_srcs})
enable_testing()
add_subdirectory(tests)
