cmake_minimum_required(VERSION 3.10)
project(webServer)

set(CMAKE_CXX_STANDARD 11)
add_subdirectory(lib/googletest)
include_directories(lib/googletest/googletest/include)
include_directories(lib/googletest/googlemock/include)

add_executable(webServer main.cpp Tests/test.cpp server/base/ThreadPool.h)
target_link_libraries(webServer gtest gtest_main)

