#include "../../includes/Socket/Socket.hpp"

Socket::Socket(const Config& conf) : _sock(-1)
{
    //creation du socket et de la struct address
    _address.sin_family = AF_INET;
    _address.sin_port = htons(conf._servers[0]._port);
    _address.sin_addr.s_addr = htonl(INADDR_ANY);
    _sock = socket(AF_INET, SOCK_STREAM, 0);
    if (_sock == -1)
        throw std::runtime_error("Could not create socket");
    // creation du bind et du listen
    if (bind(_sock, (struct sockaddr *)&_address, sizeof(_address)) == -1)
    {
        close(_sock);
        throw std::runtime_error("Could not bind socket");
    }
    if (listen(_sock, conf._servers[0]._client_max_body) == -1)
    {
        close(_sock);
        throw std::runtime_error("Could not listen on socket");
    }
    close(_sock);
}

Socket::~Socket() {}
