//
// Created by jeff on 18-6-5.
//

#include <poll.h>
#include "Channel.h"
#include "../base/Logging.h"

using namespace selfServer;
using namespace selfServer::net;
/**
 * events :
 * POLLIN 有数据可以读
 * POLLRDNORM 有普通数据可以读
 * POLLRDBAND 有优先数据可以读
 * POLLPRI 有紧急数据可以读
 * POLLWRNORM 写普通数据不会导致阻塞
 * POLLWRBAND 写优先数据不会导致阻塞
 * POLLMSGSIGPOLL 消息可用
 * revents :
 * POLLERR 指定的文件描述符发生错误
 * POLLHUP 指定的文件描述符挂起事件
 * POLLNVAL 指定的文件描述符非法
 */
const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

selfServer::net::Channel::Channel(selfServer::net::EventLoop *loop, int fd)
    : m_loop(loop),
      m_fd(fd),
      m_events(0),
      m_revents(0),
      m_index(-1)
{
}

void Channel::handleEvent()
{
    if (m_revents & POLLNVAL){
        LOG_INFO << "Channel::handleEvent() POLLNVAL";
    }

    if (m_revents & (POLLERR | POLLNVAL)){
        if (m_errorCallback) m_errorCallback();
    }

    if (m_revents & (POLLIN | POLLPRI | POLLHUP)){
        if (m_readCallback) m_readCallback();
    }

    if (m_revents & POLLOUT){
        if (m_writeCallback) m_writeCallback();
    }

}

void Channel::update()
{

}

std::string Channel::reventsToString() const
{

}

std::string Channel::eventsToString() const
{

}




