//
// Created by jeff on 18-6-25.
//

#ifndef WEBSERVER_TCPCONNECTION_H
#define WEBSERVER_TCPCONNECTION_H

#include <memory>
#include <netinet/tcp.h>
#include "../base/base.h"

namespace selfServer
{
    namespace net
    {
        class Channel;
        class EventLoop;

        class TcpConnection: public NonCopyable,
                             public std::enable_shared_from_this<TcpConnection>
        {
        public:


        private:
            enum StateE {kDisconnected, kConnecting, kConnected, kDisconnecting};
            void handleRead();


        };
    }
}


#endif //WEBSERVER_TCPCONNECTION_H
