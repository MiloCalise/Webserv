#pragma once

#include "../Client/Client.hpp"
#include "../Socket/Socket.hpp"
#include <set>
#include <vector>

class Server
{
    private:
        // constructeurs dans private pour interdire leur instanciation
        Server(const Server& copy);
        Server& operator=(const Server& copy);
        std::vector<Socket *>               _sockets;
        int                                 _epoll_fd;
        std::set<unsigned int>              _listen_fds;
        std::map<int, Client>               _clients;
        std::map<int, ServerConfig *>       _fd_to_config;

    public:
        Server();
        ~Server();
        void                startServers(const Config& conf);
        void                startLoop();
};
