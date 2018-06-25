//
// Created by jeff on 18-6-19.
//

#ifndef WEBSERVER_ACCEPTOR_H
#define WEBSERVER_ACCEPTOR_H

#include <netinet/in.h>
#include <string>
#include <functional>
#include "ChannelCallback.h"
#include "Channel.h"
#include <memory>
#include <utility>

namespace selfServer
{
    namespace net
    {
        class InetAddress;

        class Acceptor: public ChannelCallback
        {
        public:
            typedef std::function<void(int sockfd, const InetAddress&)> acceptorCallBack;

            Acceptor(EventLoop* ploop, const InetAddress& listenAddr, bool requestport);
            ~Acceptor() override ;

            void setNewConnectionCallback(acceptorCallBack callBackFunc)
            { m_callBack = std::move(callBackFunc);}

            void listen();
        private:
            void handleRead() override;

            EventLoop* mp_pLoop;
            int m_listenFd;
            int m_acceptSocketFd;
            Channel m_acceptChannel;
            acceptorCallBack m_callBack;
            bool mb_listening;
        };

    }
}
#endif //WEBSERVER_ACCEPTOR_H
