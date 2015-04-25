cmake_minimum_required(VERSION 3.0)
project(mqtt)

set(CMAKE_CXX_FLAGS "-std=c++14")

add_subdirectory(src)
add_subdirectory(test)
enable_testing()
add_test (NAME TestParser COMMAND parser)
add_test (NAME TestSerialize COMMAND serialize)
add_test (NAME TestHelper COMMAND helper)
