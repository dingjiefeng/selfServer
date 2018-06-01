//
// Created by jeff on 18-6-1.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../server/base/LogStream.h"


using namespace selfServer;
using namespace detail;

using testing::Eq;

class TestLogStream : public testing::Test
{
public:
    TestLogStream()
        : mp_ls(new LogStream()),
          mp_buffer(new FixedBuffer<kSmallBuffer>)
    {

    }

    void SetUp() override {
        std::cout << "test of LogStream begin ..." << std::endl;
    }
    std::unique_ptr<LogStream> mp_ls;
    std::unique_ptr<FixedBuffer<kSmallBuffer>> mp_buffer;
};


TEST_F(TestLogStream, fixbuffer){
    std::string text("this a text");
    *mp_ls << text;
    ASSERT_EQ(text, mp_ls->buffer().toString());
}

TEST_F(TestLogStream, logstream){
    std::string text("this a text");
    mp_buffer->append(text.c_str(), text.size());
    ASSERT_EQ(text, mp_buffer->toString());
}