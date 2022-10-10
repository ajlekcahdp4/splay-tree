set (UNIT_TEST_SOURCES
    src/main.cc
    src/test_do_tree.cc
)

if (ENABLE_GTEST)
    add_executable(unit_test ${UNIT_TEST_SOURCES})
    target_include_directories(unit_test PRIVATE ${SPLAY_TREE_INCLUDE_DIR})
    target_link_libraries (unit_test ${GTEST_BOTH_LIBRARIES})
    gtest_discover_tests(unit_test)
endif()
