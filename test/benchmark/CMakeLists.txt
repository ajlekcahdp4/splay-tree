set (SPLAY_BENCH_SOURCES
    src/main.cc
)

add_executable(benchmark ${SPLAY_BENCH_SOURCES})
target_include_directories(benchmark PRIVATE ${SPLAY_TREE_INCLUDE_DIR})
target_link_libraries(benchmark PRIVATE splay_tree)
install( TARGETS benchmark DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin)

if (BASH_PROGRAM)
    add_test (NAME test.compare_with_stl COMMAND ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/test.sh "$<TARGET_FILE:benchmark>" ${CMAKE_CURRENT_SOURCE_DIR})
endif()