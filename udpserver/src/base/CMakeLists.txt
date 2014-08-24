CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(fxbase)

list(APPEND base_srcs system_time.cc memory_list.cc mmap_file.cc)

add_library("fxbase" STATIC ${base_srcs})
#add_executable("test-memory-list" test-memory-list.cc)
#target_link_libraries("test-memory-list" "fxbase")
enable_testing()
add_subdirectory(tests)
