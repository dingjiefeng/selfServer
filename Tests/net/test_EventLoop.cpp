//
// Created by jeff on 18-6-5.
//
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../../server/net/EventLoop.h"
#include <memory>
#include <thread>

using namespace selfServer;
using namespace net;
using testing::Eq;

class TestEventLoop : public testing::Test
{
public:
    TestEventLoop() = default;

};
std::unique_ptr<EventLoop> gp_loop;

void threadFunc()
{
    gp_loop->loop();
}

//this test will assert fail
//TEST_F(TestEventLoop, isRunInLoop){
//
//    EventLoop loop;
//    gp_loop.reset(&loop);
//    std::thread t(threadFunc);
//    std::this_thread::sleep_for(std::chrono::seconds(3));
//    t.join();
//}