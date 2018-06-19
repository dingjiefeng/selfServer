//
// Created by jeff on 18-6-19.
//

#include <fcntl.h>
#include "Acceptor.h"
#include "Channel.h"

using namespace selfServer;
using namespace selfServer::net;

Acceptor::Acceptor(EventLoop *ploop)
    : m_listenFd(-1),
      m_pSockAChannel(nullptr),
      m_callBack(nullptr),
      m_pLoop(ploop)
{}

void Acceptor::start()
{
    m_listenFd = createAndListen();
    m_pSockAChannel = new Channel(m_pLoop, m_listenFd);
    m_pSockAChannel->setReadCallback(std::bind())
}

void Acceptor::setCallback(Acceptor::acceptorCallBack& callBackFunc)
{
    m_callBack.reset(new acceptorCallBack(callBackFunc));
}

void Acceptor::handleRead()
{
    int connfd;
    struct sockaddr_in cliaddr;
    socklen_t clilen = sizeof sockaddr_in;
    connfd = accept(m_listenFd, (sockaddr*)&cliaddr, (socklen_t*)&clilen);
    fcntl(connfd, F_SETFL, O_NONBLOCK);
    (*m_callBack)(connfd);
}

void Acceptor::handleWrite()
{

}


