set (QUERIES_SOURCES
    src/queries.cc
)

add_executable(queries ${QUERIES_SOURCES})
target_include_directories(queries PRIVATE ${SPLAY_TREE_INCLUDE_DIR})
target_link_libraries(queries splay_tree)

install (TARGETS queries DESTINATION ${DESTDIR} )