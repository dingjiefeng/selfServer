cmake_minimum_required(VERSION 3.10)
project(webServer)

set(CMAKE_CXX_STANDARD 11)
add_subdirectory(lib/googletest)
include_directories(lib/googletest/googletest/include)
include_directories(lib/googletest/googlemock/include)

add_executable(webServer main.cpp Tests/base/test.cpp server/base/ThreadPool.h server/base/CountDownLatch.h server/base/CountDownLatch.cpp Tests/base/test_CountDownLatch.cpp server/base/base.h server/base/LogFile.cpp server/base/LogFile.h server/base/FileUtil.cpp server/base/FileUtil.h Tests/base/test_FileUtil.cpp Tests/base/test_LogFile.cpp server/base/Logging.cpp server/base/Logging.h server/base/LogStream.cpp server/base/LogStream.h server/base/AsyncLogging.cpp server/base/AsyncLogging.h Tests/base/test_LogStream.cpp Tests/base/test_AsyncLog.cpp Tests/base/test_Logging.cpp server/net/EventLoop.cpp server/net/EventLoop.h server/net/Channel.cpp server/net/Channel.h Tests/net/test_EventLoop.cpp server/net/Poller.cpp server/net/Poller.h Tests/base/test_ThreadPool.cpp Tests/net/test_Channel.cpp server/net/EventLoopThread.cpp server/net/EventLoopThread.h server/net/Acceptor.cpp server/net/Acceptor.h server/net/InetAddress.cpp server/net/InetAddress.cpp server/net/InetAddress.h server/net/ChannelCallback.h server/net/TcpServer.cpp server/net/TcpServer.h server/net/TcpConnection.cpp server/net/TcpConnection.h server/net/EventLoopThreadPool.cpp server/net/EventLoopThreadPool.h)
target_link_libraries(webServer gtest gtest_main)

