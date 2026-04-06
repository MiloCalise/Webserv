#include "../../includes/Config/Config.hpp"
#include "../../includes/Parsing/ServerParsing.hpp"

Config::Config() {}

Config::~Config() {}

void Config::parseConfig(const std::string& file)
{
    ServerParsing parser(file);
    _servers = parser.getServers();
}

// print la config entiere
void Config::printConfig() const
{
    std::cout << "=== Configuration ===" << std::endl;
    std::cout << "Servers configured: " << _servers.size() << std::endl;
    std::cout << "=====================" << std::endl;
    for (size_t i = 0; i < _servers.size(); ++i)
    {
        std::cout << "Server [" << i << "]:" << std::endl;
        _servers[i].print();
    }
}
