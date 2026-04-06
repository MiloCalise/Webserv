#pragma once

#include "LocationConfig.hpp"
#include <string>
#include <vector>
#include <map>

class ServerParsing;
class LocationParsing;

class ServerConfig
{
    friend class ServerParsing;
    friend class LocationParsing;
    friend class Socket;

    protected :
        int                             _port;
        unsigned long                   _client_max_body;

        std::string                     _host;
        std::string                     _root;
        std::string                     _server_name;
        std::string                     _index;

        std::map<int, std::string>      _error_pages;

        std::vector<LocationConfig>     _locations;
    public :
        ServerConfig();
        ~ServerConfig();

        void addLocation(const LocationConfig& location);
        void print() const;
};
