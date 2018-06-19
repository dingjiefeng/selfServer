//
// Created by jeff on 18-6-19.
//

#ifndef WEBSERVER_CHANNELCALLBACK_H
#define WEBSERVER_CHANNELCALLBACK_H
class ChannelCallback
{
public:
    virtual void handleRead() = 0;
    virtual void handleWrite() = 0;
    virtual ~ChannelCallback() = 0;
};

#endif //WEBSERVER_CHANNELCALLBACK_H
