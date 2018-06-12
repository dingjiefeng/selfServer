//
// Created by jeff on 18-6-8.
//
#include <fcntl.h>
#include <unistd.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../../server/net/Channel.h"
#include "../../server/base/Logging.h"
#include "../../server/net/EventLoop.h"
#include <sys/timerfd.h>
#include <cstdio>
#include <cstdlib>
#include <thread>
#include <utility>

using namespace selfServer;
using namespace selfServer::net;
using testing::Eq;

class TestChannel : public testing::Test
{
public:
    TestChannel() = default;

};








