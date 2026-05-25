#include "../../includes/Socket/Socket.hpp"
#include <sys/socket.h>
#include <cstring>
#include <cerrno>

static std::string makeSystemError(const std::string& prefix)
{
    return prefix + ": " + std::strerror(errno);
}

Socket::Socket(const ServerConfig& conf) : _sock(-1)
{
    if (conf._port <= 0 || conf._port > 65535)
        throw std::runtime_error("Invalid server port");

    std::memset(&_address, 0, sizeof(_address));
    _address.sin_family = AF_INET;
    _address.sin_port = htons(conf._port);
    _address.sin_addr.s_addr = htonl(INADDR_ANY);
    _sock = socket(AF_INET, SOCK_STREAM, 0);
    if (_sock == -1)
        throw std::runtime_error(makeSystemError("Could not create socket"));

    int reuse = 1;
    if (setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1)
    {
        close(_sock);
        throw std::runtime_error(makeSystemError("Could not set SO_REUSEADDR"));
    }
    // creation du bind et du listen
    if (bind(_sock, (struct sockaddr *)&_address, sizeof(_address)) == -1)
    {
        close(_sock);
        throw std::runtime_error(makeSystemError("Could not bind socket"));
    }
    if (listen(_sock, SOMAXCONN) == -1)
    {
        close(_sock);
        throw std::runtime_error(makeSystemError("Could not listen on socket"));
    }
}

Socket::~Socket()
{
    if (_sock != -1)
        close(_sock);
}
