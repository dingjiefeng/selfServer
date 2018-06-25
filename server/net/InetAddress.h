//
// Created by jeff on 18-6-19.
//

#ifndef WEBSERVER_INETADDRESS_H
#define WEBSERVER_INETADDRESS_H

#include <string>
#include <netinet/in.h>

namespace selfServer
{
    namespace net
    {
        /**
         * wrapper of sockaddr_in
         * 具有值语义
         */
        class InetAddress
        {
        public:
            explicit InetAddress(uint16_t port = 0, bool loopbackOnly = false);

            //允许使用隐式类型转换
            InetAddress(std::string ip, uint16_t port = 0);
            explicit InetAddress(const struct sockaddr_in& addr)
                    : m_addr(addr)
            {}

            sa_family_t family() const { return m_addr.sin_family;}
            std::string toIp() const;
//            std::string toIpPort() const;
//            uint16_t toPort() const;

//            u_int32_t ipNetEndian() const;
            uint16_t portNetEndian() const { return m_addr.sin_port;}
            // resolve hostname to IP address, not changing port or sin_family
            // return true on success.
            // thread safe
            static bool resolve(std::string hostname, InetAddress* out);
//        private:
            union
            {
                sockaddr_in m_addr;
                sockaddr_in6 m_addr6;
            };

        };
    }
}
#endif //WEBSERVER_INETADDRESS_H
