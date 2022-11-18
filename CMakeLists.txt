cmake_minimum_required(VERSION 3.14)

project (splay-tree)

option(INSOURCEBUILD OFF)
if((${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR}) AND NOT ${INSOURCEBUILD})
    message(FATAL_ERROR "In-source building disabled. Provide -DINSOURCEBUILD option if you are sure about it.")
endif()

set (CMAKE_CXX_STANDARD 20)

add_compile_options(-Wall -Wextra -O2)

set (SPLAY_TREE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/include)

find_package(GTest)

set(NOGTEST FALSE ON CACHE BOOL "Disable GoogleTest")
set (COMPARE FALSE ON CACHE BOOL "Disable comparison with std::set")
if (COMPARE)
    message(WARNING "Comparation with std::test enabled. It should take a lot of time. If you want to disable it rerun with -DCOMPARE=False")
endif()

enable_testing()
if(NOT NOGTEST AND GTEST_FOUND AND CMAKE_MINOR_VERSION GREATER_EQUAL 20)
    set(ENABLE_GTEST ON)
else()
    message(WARNING "Google Test disabled")
endif()

set(DCMAKE_EXPORT_COMPILE_COMMANDS ON)

option(SANITIZE OFF)
if (SANITIZE)
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address -fno-omit-frame-pointer)
endif()

find_program (BASH_PROGRAM bash)

add_subdirectory(lib)
add_subdirectory(test)