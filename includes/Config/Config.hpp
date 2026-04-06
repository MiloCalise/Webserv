#pragma once

#include "ServerConfig.hpp"
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
    std::vector<ServerConfig> _servers;
    public:
        Config();
        ~Config();
        void    parseConfig(const std::string& file);
        void    printConfig() const;
};
