//
// Created by jeff on 18-6-19.
//

#include "InetAddress.h"
#include <netdb.h>
#include <memory.h>
#include <arpa/inet.h>

using namespace selfServer;
using namespace selfServer::net;

InetAddress::InetAddress(uint16_t port, bool loopbackOnly)
{
    bzero(&m_addr, sizeof m_addr);
    m_addr.sin_family = AF_INET;
    in_addr_t ip = loopbackOnly ? INADDR_LOOPBACK : INADDR_ANY;
    m_addr.sin_addr.s_addr = htonl(ip);
    m_addr.sin_port = htons(port);
}

InetAddress::InetAddress(std::string ip, uint16_t port)
{
    bzero(&m_addr, sizeof m_addr);
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    m_addr.sin_port = htons(port);
}

std::string InetAddress::toIp() const
{
    char buf[64] = "";
    size_t size = sizeof buf;
    const auto * addr4 = static_cast<const struct sockaddr_in*>(reinterpret_cast<const void*>(&m_addr));
    ::inet_ntop(AF_INET, &addr4->sin_addr, buf, static_cast<socklen_t>(size));
    return std::string(buf);
}






//global
static __thread char t_resolveBuffer[64 * 1024];
bool InetAddress::resolve(std::string hostname, InetAddress *out)
{
    struct hostent hent;
    struct hostent* he = nullptr;
    int herrno = 0;
    bzero(&hent, sizeof(hent));

    int ret = gethostbyname_r(hostname.c_str(), &hent, t_resolveBuffer, sizeof t_resolveBuffer, &he, &herrno);
    if (ret == 0 && he != nullptr)
    {
        out->m_addr.sin_addr = *reinterpret_cast<struct in_addr*>(he->h_addr);
        return true;
    }
    else
    {
        return false;
    }
}