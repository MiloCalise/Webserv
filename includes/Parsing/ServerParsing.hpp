#pragma once

#include "LocationParsing.hpp"
#include <vector>

class ServerParsing : public LocationParsing
{
    private :
        ServerParsing(const ServerParsing& copy);
        ServerParsing& operator=(const ServerParsing& copy);
        
    protected :
        std::vector<ServerConfig> _servers;

        void    parse();
        void    parse(ServerConfig& current_serv);
        
    public :
        ServerParsing(const std::string& path);
        ~ServerParsing();
        
        const std::vector<ServerConfig>& getServers() const;
};