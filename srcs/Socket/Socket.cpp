#include "../../includes/Socket/Socket.hpp"

Socket::Socket()
{
    // creation du socket et de la struct address
    // _address.sin_family = AF_INET;
    // _address.sin_port = htons(_port);
    // _address.sin_addr.s_addr = htonl(_interface);
    // _sock = socket(AF_INET, SOCK_STREAM, 0);
    // if (_sock == -1)
    //     throw std::runtime_error("Could not create socket");
    // // creation du bind et du listen
    // if (bind(_sock, (struct sockaddr *)&_address, sizeof(_address)) == -1)
    //     throw std::runtime_error("Could not bind socket");
    // if (listen(_sock, _backlog) == -1)
    //     throw std::runtime_error("Could not listen on socket");
}

Socket::~Socket() {}
