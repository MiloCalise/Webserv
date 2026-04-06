#include "../../includes/Config/ServerConfig.hpp"

ServerConfig::ServerConfig() : _port(0), _client_max_body(0)
{
}

ServerConfig::~ServerConfig()
{
}

void ServerConfig::addLocation(const LocationConfig& location)
{
    _locations.push_back(location);
}

void ServerConfig::print() const
{
    std::cout << "Server Name: " << _server_name << std::endl;
    std::cout << "Port: " << _port << std::endl;
    std::cout << "Client Max Body Size: " << _client_max_body << std::endl;
    
    if (!_error_pages.empty())
    {
        std::cout << "Error Pages:" << std::endl;
        for (std::map<int, std::string>::const_iterator it = _error_pages.begin(); it != _error_pages.end(); ++it)
            std::cout << "  " << it->first << " -> " << it->second << std::endl;
    }

    std::cout << "Locations (" << _locations.size() << "):" << std::endl;
    for (size_t i = 0; i < _locations.size(); ++i)
    {
        _locations[i].print();
    }
    std::cout << "-----------------------------------" << std::endl;
}