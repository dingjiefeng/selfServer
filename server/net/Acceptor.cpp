//
// Created by jeff on 18-6-19.
//

#include <fcntl.h>
#include <memory.h>
#include "Acceptor.h"
#include "Channel.h"
#include "InetAddress.h"
#include "EventLoop.h"

using namespace selfServer;
using namespace selfServer::net;

int createNonblockingOrDie(sa_family_t family)
{
    int sockfd = ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC , IPPROTO_TCP);
    if (sockfd < 0)
    {
        std::string str("fail to create sockfd in ");
        str += "file:" + std::string(__FILE__) + " line:" + std::to_string(__LINE__)
               + " func:" + std::string(__func__);
        perror(str.c_str());
        exit(-1);
    }
}

Acceptor::Acceptor(EventLoop *ploop, const InetAddress &listenAddr, bool requestport)
    : mp_pLoop(ploop),
      m_acceptSocketFd(createNonblockingOrDie(listenAddr.family())),
      m_acceptChannel(ploop, m_acceptSocketFd),
      mb_listening(false)
{
    int opt_val = requestport ? 1 : 0;
    setsockopt(m_acceptSocketFd, SOL_SOCKET, SO_REUSEADDR,
                 &opt_val, static_cast<socklen_t >(sizeof(opt_val)));
    setsockopt(m_acceptSocketFd, SOL_SOCKET, SO_REUSEPORT,
                 &opt_val, static_cast<socklen_t >(sizeof(opt_val)));

}

Acceptor::~Acceptor()
{
    m_acceptChannel.disableAll();
    m_acceptChannel.remove();
}

void Acceptor::listen()
{
    mp_pLoop->assertInLoopThread();
    mb_listening = true;
    //listen
    int ret = ::listen(m_acceptSocketFd, SOMAXCONN);
    if (ret < 0)
    {
        std::string str("can not create listen fd ");
        str += "file:" + std::string(__FILE__) + " line:" + std::to_string(__LINE__)
                + " func:" + std::string(__func__);
        perror(str.c_str());
        exit(-1);
    }
    m_acceptChannel.enableReading();
}

void Acceptor::handleRead()
{
    mp_pLoop->assertInLoopThread();
    InetAddress peerAddr;
    struct sockaddr_in6 addr{};
    bzero(&addr, sizeof addr);
    auto addrlen = static_cast<socklen_t >(sizeof addr);
    int connfd = accept4(m_acceptSocketFd, static_cast<struct sockaddr*>(&addr),
                 &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (connfd >= 0)
    {
        peerAddr.m_addr6 = addr;
        if (m_callBack)
        {
            m_callBack(connfd, peerAddr);
        } else
        {
            ::close(connfd);
        }
    } else
    {
        std::string str("can not create conn fd ");
        str += "file:" + std::string(__FILE__) + " line:" + std::to_string(__LINE__)
               + " func:" + std::string(__func__);
        perror(str.c_str());
        exit(-1);
    }

}
