#pragma once

#include "Parsing.hpp"

class LocationParsing : public Parsing
{
    private :
        LocationParsing(const LocationParsing& copy);
        LocationParsing& operator=(const LocationParsing& copy);
    protected :
        void    parse();
        void    parse(ServerConfig& current_serv);
    public :
        LocationParsing(const std::string& path);
        ~LocationParsing();
};