//
// Created by jeff on 18-6-1.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../../server/base/LogStream.h"
#include <thread>

using namespace selfServer;
using namespace detail;
using std::string;
using testing::Eq;

class TestLogStream : public testing::Test
{
public:
    TestLogStream()
        : mp_ls(new LogStream()),
          mp_buffer(new FixedBuffer<kSmallBuffer>)
    {

    }

    std::unique_ptr<LogStream> mp_ls;
    std::unique_ptr<FixedBuffer<kSmallBuffer>> mp_buffer;
    const size_t N = 1000000;
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

TEST_F(TestLogStream, testLogStreamBooleans)
{
    LogStream os;
    const LogStream::Buffer &buf = os.buffer();
    ASSERT_EQ(buf.toString(), std::string(""));
    os << true;
    ASSERT_EQ(buf.toString(), std::string("1"));
    os << '\n';
    ASSERT_EQ(buf.toString(), std::string("1\n"));
    os << false;
    ASSERT_EQ(buf.toString(), std::string("1\n0"));
}

TEST_F(TestLogStream, testLogStreamIntegers)
{
    LogStream os;
    const LogStream::Buffer &buf = os.buffer();
    ASSERT_EQ(buf.toString(), std::string(""));
    os << 1;
    ASSERT_EQ(buf.toString(), std::string("1"));
    os << 0;
    ASSERT_EQ(buf.toString(), std::string("10"));
    os << -1;
    ASSERT_EQ(buf.toString(), std::string("10-1"));
    os.resetBuffer();

    os << 0 << " " << 123 << 'x' << 0x64;
    ASSERT_EQ(buf.toString(), std::string("0 123x100"));
}

TEST_F(TestLogStream, testLogStreamIntegerLimits)
{
    LogStream os;
    const LogStream::Buffer &buf = os.buffer();
    ASSERT_EQ(buf.toString(), std::string(""));
    os << -2147483647;
    ASSERT_EQ(buf.toString(), string("-2147483647"));
    os << static_cast<int>(-2147483647 - 1);
    ASSERT_EQ(buf.toString(), string("-2147483647-2147483648"));
    os << ' ';
    os << 2147483647;
    ASSERT_EQ(buf.toString(), string("-2147483647-2147483648 2147483647"));
    os.resetBuffer();

    os << std::numeric_limits<int16_t>::min();
    ASSERT_EQ(buf.toString(), string("-32768"));
    os.resetBuffer();

    os << std::numeric_limits<int16_t>::max();
    ASSERT_EQ(buf.toString(), string("32767"));
    os.resetBuffer();

    os << std::numeric_limits<uint16_t>::min();
    ASSERT_EQ(buf.toString(), string("0"));
    os.resetBuffer();

    os << std::numeric_limits<uint16_t>::max();
    ASSERT_EQ(buf.toString(), string("65535"));
    os.resetBuffer();

    os << std::numeric_limits<int32_t>::min();
    ASSERT_EQ(buf.toString(), string("-2147483648"));
    os.resetBuffer();

    os << std::numeric_limits<int32_t>::max();
    ASSERT_EQ(buf.toString(), string("2147483647"));
    os.resetBuffer();

    os << std::numeric_limits<uint32_t>::min();
    ASSERT_EQ(buf.toString(), string("0"));
    os.resetBuffer();

    os << std::numeric_limits<uint32_t>::max();
    ASSERT_EQ(buf.toString(), string("4294967295"));
    os.resetBuffer();

    os << std::numeric_limits<int64_t>::min();
    ASSERT_EQ(buf.toString(), string("-9223372036854775808"));
    os.resetBuffer();

    os << std::numeric_limits<int64_t>::max();
    ASSERT_EQ(buf.toString(), string("9223372036854775807"));
    os.resetBuffer();

    os << std::numeric_limits<uint64_t>::min();
    ASSERT_EQ(buf.toString(), string("0"));
    os.resetBuffer();

    os << std::numeric_limits<uint64_t>::max();
    ASSERT_EQ(buf.toString(), string("18446744073709551615"));
    os.resetBuffer();

    int16_t a = 0;
    int32_t b = 0;
    int64_t c = 0;
    os << a;
    os << b;
    os << c;
    ASSERT_EQ(buf.toString(), string("000"));
}

TEST_F(TestLogStream, testLogStreamFloats)
{
    LogStream os;
    const LogStream::Buffer &buf = os.buffer();
    ASSERT_EQ(buf.toString(), std::string(""));
    os << 0.0;
    ASSERT_EQ(buf.toString(), string("0"));
    os.resetBuffer();

    os << 1.0;
    ASSERT_EQ(buf.toString(), string("1"));
    os.resetBuffer();

    os << 0.1;
    ASSERT_EQ(buf.toString(), string("0.1"));
    os.resetBuffer();

    os << 0.05;
    ASSERT_EQ(buf.toString(), string("0.05"));
    os.resetBuffer();

    os << 0.15;
    ASSERT_EQ(buf.toString(), string("0.15"));
    os.resetBuffer();

    double a = 0.1;
    os << a;
    ASSERT_EQ(buf.toString(), string("0.1"));
    os.resetBuffer();

    double b = 0.05;
    os << b;
    ASSERT_EQ(buf.toString(), string("0.05"));
    os.resetBuffer();

    double c = 0.15;
    os << c;
    ASSERT_EQ(buf.toString(), string("0.15"));
    os.resetBuffer();

    os << a+b;
    ASSERT_EQ(buf.toString(), string("0.15"));
    os.resetBuffer();

    ASSERT_FALSE(a+b == c);

    os << 1.23456789;
    ASSERT_EQ(buf.toString(), string("1.23456789"));
    os.resetBuffer();

    os << 1.234567;
    ASSERT_EQ(buf.toString(), string("1.234567"));
    os.resetBuffer();

    os << -123.456;
    ASSERT_EQ(buf.toString(), string("-123.456"));
}

TEST_F(TestLogStream, testLogStreamStrings)
{
    LogStream os;
    const LogStream::Buffer& buf = os.buffer();

    os << "Hello ";
    ASSERT_EQ(buf.toString(), string("Hello "));

    string str = "NJUST";
    os << str;
    ASSERT_EQ(buf.toString(), string("Hello NJUST"));
}

TEST_F(TestLogStream, testLogStreamLong)
{
    LogStream os;
    const LogStream::Buffer &buf = os.buffer();
    for (int i = 0; i < 399; ++i)
    {
        os << "123456789 ";
        ASSERT_EQ(buf.length(), 10*(i+1));
        ASSERT_EQ(buf.avail(), 4000 - 10*(i+1));
    }
    os << "abcdefghi ";
    ASSERT_EQ(buf.length(), 3990);
    ASSERT_EQ(buf.avail(), 10);

    os << "abcdefghi";
    ASSERT_EQ(buf.length(), 3999);
    ASSERT_EQ(buf.avail(), 1);
}

TEST_F(TestLogStream, testPrintfBench)
{
    char buf[32];
    MyTime::timeUnit tu = MyTime::timeUnit::millisecond;
    MyTime m;
    m.mark();
    for (int i = 0; i < N; ++i) {
        snprintf(buf, sizeof buf, "%d", i);
    }
    printf("benchPrintf %f %s\n\n", m.getDuration(tu), m.time2str(tu).c_str());
}

TEST_F(TestLogStream, testStringSTreamBench)
{
    MyTime m;
    MyTime::timeUnit tu = MyTime::timeUnit::millisecond;
    std::ostringstream os;
    m.mark();
    for (size_t i = 0; i < N; ++i)
    {
        os << i;
        os.seekp(0, std::ios_base::beg);
    }
    printf("benchStringStream %f %s\n\n", m.getDuration(tu), m.time2str(tu).c_str());
}

TEST_F(TestLogStream, testLogStreamBench)
{
    MyTime m;
    MyTime::timeUnit tu = MyTime::timeUnit::millisecond;
    LogStream os;
    m.mark();
    for (size_t i = 0; i < N; ++i)
    {
        os << (i);
        os.resetBuffer();
    }
    printf("benchLogStream %f %s\n\n", m.getDuration(tu), m.time2str(tu).c_str());
}