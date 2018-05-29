cmake_minimum_required(VERSION 3.10)
project(webServer)

set(CMAKE_CXX_STANDARD 11)
add_subdirectory(lib/googletest)
include_directories(lib/googletest/googletest/include)
include_directories(lib/googletest/googlemock/include)

add_executable(webServer main.cpp Tests/test.cpp server/base/ThreadPool.h server/base/CountDownLatch.h server/base/CountDownLatch.cpp Tests/test_CountDownLatch.cpp server/base/base.h server/base/LogFile.cpp server/base/LogFile.h server/base/FileUtil.cpp server/base/FileUtil.h Tests/test_FileUtil.cpp)
target_link_libraries(webServer gtest gtest_main)

