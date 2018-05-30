//
// Created by jeff on 18-5-30.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../server/base/LogFile.h"

using testing::Eq;
using namespace selfServer;

class TestLogFile : public testing::Test
{
public:
    TestLogFile()
            :mp_lf(new LogFile("/home/jeff/Desktop/selfServer/Tests/FileLogDemo.txt", 2))
    {

    }

    void SetUp() override {
        std::cout << "test of LogFile begin..." << std::endl;
    }
    std::unique_ptr<LogFile> mp_lf;
};

TEST_F(TestLogFile, demo){
    std::string txt("a demo file\n");
    mp_lf->append(txt.c_str(), static_cast<int>(txt.size()));
    mp_lf->append(txt.c_str(), static_cast<int>(txt.size()));
    mp_lf->append(txt.c_str(), static_cast<int>(txt.size()));
    mp_lf->append(txt.c_str(), static_cast<int>(txt.size()));
}
