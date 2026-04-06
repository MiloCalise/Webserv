#pragma once

#include <vector>
#include <map>
#include <iostream>

class LocationParsing;
class ServerParsing;

class LocationConfig
{
    friend class LocationParsing;
    friend class ServerParsing;

private :
    LocationConfig();
    
protected :
    std::string                         _path;
    std::string                         _root;
    std::string                         _index;
    std::string                         _upload;
    std::string                         _redirect;

    std::vector<std::string>            _methods;

    std::map<std::string, std::string>  _cgi;

    bool                                _autoindex;
    
public :
    LocationConfig(std::string path);
    ~LocationConfig();

    void print() const;
};