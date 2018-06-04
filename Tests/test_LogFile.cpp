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
    {

    }

    void SetUp() override {
        std::cout << "test of LogFile begin..." << std::endl;
    }
    std::unique_ptr<LogFile> mp_lf;
};

TEST_F(TestLogFile, demo){
    std::string basename("/home/jeff/Desktop/selfServer/Tests/Log/LogFileDemo");
    mp_lf.reset(new LogFile(basename, 20*100));
    std::string txt("a demo file\n");
    mp_lf->append(txt.c_str(), static_cast<int>(txt.size()));
    mp_lf->append(txt.c_str(), static_cast<int>(txt.size()));
    mp_lf->append(txt.c_str(), static_cast<int>(txt.size()));
    mp_lf->append(txt.c_str(), static_cast<int>(txt.size()));
}

TEST_F(TestLogFile, run){
    std::string basename("/home/jeff/Desktop/selfServer/Tests/Log/LogFileDemo");
    mp_lf.reset(new LogFile(basename, 20*100));
    std::string line("1234567890 abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ ");
    for (int i = 0; i < 1000; ++i)
    {
        mp_lf->append(line.c_str(), line.size());
        mp_lf->append(std::to_string(i).c_str(), std::to_string(i).size());
        mp_lf->append("\n", 1);
//        usleep(1000);
    }
}