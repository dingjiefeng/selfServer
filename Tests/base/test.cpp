//
// Created by jeff on 18-5-23.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../../server/base/ThreadPool.h"

using testing::Eq;


class testDemo : public testing::Test{
public:
    testDemo()
    = default;
};

TEST_F(testDemo, nameOfTheTest1){
    try{
        ASSERT_EQ("", "");
    } catch (std::exception& e){
        std::cout << e.what() << std::endl;
    }
}

TEST_F(testDemo, nameOfTheTest2){
    ASSERT_EQ(0, 0);
}

TEST(ThreadPool, example){
    int num(10);
    selfServer::ThreadPool tp(num);
    for (int i = 0; i < num; ++i)
    {
        tp.add([](int number){ std::cout << number << std::endl;}, i);
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ASSERT_EQ(1, 1);
}