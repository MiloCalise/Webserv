#pragma once

#include "../Config/Config.hpp"
#include "../Client/Request.hpp"

class   Client
{
    friend class Server;
    private:
        Client(const Client& copy);
        Client& operator=(const Client& copy);

        int             _fd;
        ServerConfig    *_config;
        time_t          _last_activity;
        std::string     _read_buffer;
        std::string     _write_buffer;
        size_t          _bytes_sent;
        Request         _request;

    public:
        Client();
        Client(int fd, ServerConfig *conf);
        ~Client();

        void                appendToReadBuffer(const std::string& data);
        bool                tryParseRequest();
        bool                isRequestComplete() const;
        bool                hasDataToSend() const;

        int                 getFd() const;
        Request&            getRequest();
        ServerConfig*       getConfig();
        time_t              getLastActivity() const;

        void                setWriteBuffer(const std::string& response);
        ssize_t             sendChunk();
};
