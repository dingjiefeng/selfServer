//
// Created by jeff on 18-6-2.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../server/base/AsyncLogging.h"
#include "../server/base/Logging.h"

using namespace selfServer;

class TestAsyncLog : public testing::Test
{
public:
    TestAsyncLog()
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

TEST_F(TestAsyncLog, asynclog)
{
    size_t kOneMB = 1024*1024;

    mp_asynclog.reset(new AsyncLogging(Logger::g_logFileName, kOneMB));

    printf("pid = %d\n", getpid());

    mp_asynclog->start();

    std::string text("hello 123456789\n");
    const int kBatch = 100000;
    for (int i = 0; i < kBatch; ++i) {
        mp_asynclog->append(text.c_str(), static_cast<int>(text.size()));
        mp_asynclog->append(std::to_string(i).c_str(), static_cast<int>(std::to_string(i).size()));
        mp_asynclog->append("/n", 1);
        usleep(1);
    }
    usleep(1);
}