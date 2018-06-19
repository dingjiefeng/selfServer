//
// Created by jeff on 18-6-19.
//

#ifndef WEBSERVER_ACCEPTOR_H
#define WEBSERVER_ACCEPTOR_H

#include <netinet/in.h>
#include <string>
#include <functional>
#include "ChannelCallback.h"
#include <memory>

namespace selfServer
{
    namespace net
    {
        class EventLoop;
        class InetAddress;
        class Channel;

        class Acceptor: public ChannelCallback
        {
        public:
            typedef std::function<void(int sockfd)> acceptorCallBack;

            explicit Acceptor(EventLoop* ploop);
            ~Acceptor() override = default;

            void start();
            void setCallback(acceptorCallBack& callBackFunc);
            void handleRead() override;
            void handleWrite() override;
        private:
            int createAndListen();

            int m_listenFd;
            Channel* m_pSockAChannel;
            std::unique_ptr<acceptorCallBack> m_callBack;
            EventLoop* m_pLoop;
        };

    }
}
#endif //WEBSERVER_ACCEPTOR_H
