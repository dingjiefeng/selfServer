//
// Created by jeff on 18-6-2.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../server/base/AsyncLogging.h"

using namespace selfServer;

class TestAsyncLog : public testing::Test
{
public:
    TestAsyncLog()
            : mp_asynclog(new AsyncLogging("/home/jeff/Desktop/selfServer/Tests/AsyncLogDemo"))
    {}

    void SetUp() override {
        std::cout << "test of asynclogging begin..." << std::endl;
    }

    std::unique_ptr<AsyncLogging> mp_asynclog;
};

TEST_F(TestAsyncLog, condition_variable_wait_for){
    size_t time1 = 4;
    size_t time2 = 2;
    const std::string timeout("time is up");
    const std::string condNotify("cond notify");
    std::mutex mu;
    std::unique_lock<std::mutex> lock(mu);
    std::condition_variable cond;
    std::thread t([&cond, &time1](){
        std::this_thread::sleep_for(std::chrono::seconds(time1));
        cond.notify_one();
    });
    auto ret = cond.wait_for(lock, std::chrono::seconds(time2));
    std::string str;
    if (ret == std::cv_status::timeout)
    {
        str = timeout;
    } else
    {
        str = condNotify;
    }

    t.join();
    ASSERT_EQ(str, time1<time2 ? condNotify : timeout);
}

TEST_F(TestAsyncLog, asynclog){

}