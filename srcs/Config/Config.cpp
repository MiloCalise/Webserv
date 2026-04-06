#include "../../includes/Socket/Socket.hpp"

Config::Config() {}

Config::~Config() {}

void    Config::parseConfig(const std::string& conf)
{
    std::ifstream   file;
    std::string     buff;

    file.open(conf.c_str(), std::ifstream::in);
    if (!file.is_open())
        throw std::runtime_error("Could not open config file");
    while (file.good())
        {
            std::getline(file, buff);
            // parser chaque ligne et mettre dans la classe
        }
    file.close();
}
