#pragma once

#include "../Config/Config.hpp"
#include "../Client/Request.hpp"

class   Client
{
    private:
        Client();
        Client(const Client& copy);
        Client& operator=(const Client& copy);

        int             _fd;
        ServerConfig    *_serv;
        time_t          _last_activity;
        std::string     _read_buffer;
        std::string     _write_buffer;
        bool            _header_read;
        size_t          _length;
        size_t          _bytes_sent;
        Request         _request;

    public:
        Client(int fd, ServerConfig *conf);
        ~Client();
};
