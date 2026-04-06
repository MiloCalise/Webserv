#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>

class Config
{
    private:
        // constructeurs dans private pour interdire leur instanciation
        Config(const Config& copy);
        Config& operator=(const Config& copy);
    protected:
        int             _backlog;
        unsigned long   _interface;
        int             _port;
        std::string     _name;
        // etc..
    public:
        Config();
        ~Config();
        void    parseConfig(const std::string& file);
};
