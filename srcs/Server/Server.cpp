#include "../../includes/Server/Server.hpp"
#include <sys/epoll.h>

Server::Server() : _epoll_fd(-1) {}

Server::~Server()
{
    for (size_t i = 0; i < _sockets.size(); i++)
        delete _sockets[i];
    if (_epoll_fd != -1)
        close(_epoll_fd);
}

void    Server::startServers(const Config& conf)
{
    for (unsigned long i = 0; i < conf._servers.size(); i++)
    {
        Socket  *s = new Socket(conf._servers[i]);
        _sockets.push_back(s);
    }
    // argument de create ignore
    _epoll_fd = epoll_create(1);
    if (_epoll_fd == -1)
        throw std::runtime_error("Could not create epoll instance");
    for (unsigned long i = 0; i < conf._servers.size(); i++)
    {
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = _sockets[i]->_sock;
        if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _sockets[i]->_sock, &ev) == -1)
            throw std::runtime_error("Could not add socket to epoll instance");
    }
}

void    Server::startLoop()
{

}
