set (SPLAY_TREE_LIB_SOURCES
    src/do_tree.cc
)

add_library(splay_tree ${SPLAY_TREE_LIB_SOURCES})
target_include_directories(splay_tree PUBLIC include)

add_subdirectory(test)
