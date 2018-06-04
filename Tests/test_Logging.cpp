//
// Created by jeff on 18-6-4.
//

#include <thread>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../server/base/Logging.h"
#include "../server/base/ThreadPool.h"
#include "../server/base/CountDownLatch.h"

using testing::Eq;
using namespace selfServer;
class TestLogging : public testing::Test
{
public:
    TestLogging() = default;

    void SetUp() override {

    }
};

TEST_F(TestLogging, bench){
    const size_t N = 100;
    CountDownLatch latch(3);
    ThreadPool tp(3);
    for (int i = 0; i < 3; ++i) {
        tp.add([N, i, &latch](){
            for (int j = 0; j < N; ++j) {
                LOG << "thread " << i << ", num:" << j << " ";
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            latch.countDown();
        });
    }
    latch.wait();
}