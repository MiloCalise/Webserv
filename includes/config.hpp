#pragma once

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <sys/epoll.h>

class Config
{
    private:
        int         _listen;
        std::string  _name;
        // etc..
    public:
        Config();
        ~Config();
        void    parseConfig(const std::string& file);
};
