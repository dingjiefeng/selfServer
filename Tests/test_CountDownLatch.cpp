//
// Created by jeff on 18-5-24.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <iostream>
#include "../server/base/CountDownLatch.h"
#include "../server/base/ThreadPool.h"


using testing::Eq;

class TestCountDownLatch : public testing::Test{
public:
//    TestCountDownLatch(): m_cdl(2)
//    {}

    void SetUp() override {
        std::cout << "start to Test..." << std::endl;
        mp_cdl = new CountDownLatch(2);
    }

    CountDownLatch* mp_cdl;
};

TEST_F(TestCountDownLatch, demo){
    selfServer::ThreadPool tp(2);
    tp.add([&]()
           {
               std::cout << "1st thread running" << std::endl;
               mp_cdl->countDown();
               std::cout << "1st thread count down, then wait" << std::endl;
               mp_cdl->wait();
               std::cout << "1st thread completed" << std::endl;
           }
    );
    tp.add([&]()
           {
               std::cout << "3rd thread running" << std::endl;
               std::cout << "3rd thread count down" << std::endl;
               mp_cdl->countDown();
           }
    );
    mp_cdl->wait();
//    std::this_thread::sleep_for(std::chrono::seconds(2));
    tp.stop();
    ASSERT_EQ(1,1);
}