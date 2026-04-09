#pragma once

#include <cstddef>
#include <iostream>
#include <map>

class Request
{
    private:
        Request(const Request& copy);
        Request& operator=(const Request& copy);
        std::string                         _method;
        std::string                         _path;
        std::string                         _version;
        std::map<std::string, std::string>  _headers;
        std::string                         _body;
        size_t                              _content_length;
        bool                                _complete;
        bool                                _parsed_header;

    public:
        Request();
        ~Request();
};
