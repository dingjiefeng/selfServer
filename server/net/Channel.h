//
// Created by jeff on 18-6-5.
//

#ifndef WEBSERVER_CHANNEL_H
#define WEBSERVER_CHANNEL_H


#include "../base/base.h"
#include <functional>

namespace selfServer
{
    namespace net
    {
        class EventLoop;
        /**
         * @brief
         */
        class Channel : public NonCopyable
        {
        public:
            typedef std::function<void()> EventCallback;

            Channel(EventLoop* loop, int fd);

            void handleEvent();

            void setReadCallback(const EventCallback& cb)
            { m_readCallback = cb;}

            void setWriteCallback(const EventCallback& cb)
            { m_writeCallback = cb;}

            void setErrorCallback(const EventCallback& cb)
            { m_errorCallback = cb;}

            int fd() const { return m_fd;}

            int events() const { return m_events;}

            void set_revents(int revt) { m_revents = revt;}

            bool isNoneEvent() const { return m_events == kNoneEvent;}

            void enableReading() { m_events |= kReadEvent; update();}

            //for poller
            int index() const { return m_index;}
            void set_index(const int idx) { m_index = idx;}

            EventLoop* ownerLoop() { return m_loop;}

        private:

            void update();

            static const int kNoneEvent;
            static const int kReadEvent;
            static const int kWriteEvent;

            EventLoop* m_loop;
            const int m_fd;
            int m_events;
            int m_revents;
            int m_index;

            EventCallback m_readCallback;
            EventCallback m_writeCallback;
            EventCallback m_errorCallback;
        };
    }
}
#endif //WEBSERVER_CHANNEL_H
