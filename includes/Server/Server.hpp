#pragma once

#include "../Socket/Socket.hpp"

class Server
{
    private:
        // constructeurs dans private pour interdire leur instanciation
        Server(const Server& copy);
        Server& operator=(const Server& copy);
        std::vector<Socket *> _sockets;
        int                 _epoll_fd;

    public:
        Server();
        ~Server();
        void                startServers(const Config& conf);
        void                startLoop();
};
