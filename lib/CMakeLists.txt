set (SPLAY_TREE_LIB_SOURCES
    src/tree_node.cc
    src/binary_tree.cc
)

add_library(splay_tree ${SPLAY_TREE_LIB_SOURCES})
target_include_directories(splay_tree PUBLIC include)

add_subdirectory(test)
