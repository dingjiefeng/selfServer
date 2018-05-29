#include <iostream>
#include <string>
#include "./server/base/ThreadPool.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace selfServer;
void tpDemo()
{
    std::mutex mtx;
    try
    {
        ThreadPool tp;
        std::vector<std::future<int>> v;
        std::vector<std::future<void>> v1;
        for (int i = 0; i <= 10; ++i)
        {
            auto ans = tp.add([](int answer) { return answer; }, i);
            v.push_back(std::move(ans));
        }
        for (int i = 0; i <= 5; ++i)
        {
            auto ans = tp.add([&mtx](const std::string& str1, const std::string& str2)
                              {
                                  std::lock_guard<std::mutex> lg(mtx);
                                  std::cout << (str1 + str2) << std::endl;
                                  return;
                              }, "hello ", "world");
            v1.push_back(std::move(ans));
        }
        for (auto &i : v) {
            std::lock_guard<std::mutex> lg(mtx);
            std::cout << i.get() << std::endl;
        }
        for (auto &i : v1) {
            i.get();
        }
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();

    return 0;
}