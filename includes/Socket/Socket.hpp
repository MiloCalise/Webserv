#pragma once

#include "../Config/Config.hpp"
#include <unistd.h>

class   Socket
{
    private:
        // constructeurs dans private pour interdire leur instanciation
        Socket(const Socket& copy);
        Socket& operator=(const Socket& copy);

        struct sockaddr_in      _address;
        int                     _sock;

    public:
        Socket(const Config& conf);
        ~Socket();
};
