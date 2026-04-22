#pragma once

#include <cstddef>
#include <sstream>
#include <iostream>
#include <map>

class Request
{
    friend class Client;
    friend class Server;
    friend class Response;
    private:
        std::string                         _method;
        std::string                         _path;
        std::string                         _version;
        std::map<std::string, std::string>  _headers;
        std::string                         _body;
        size_t                              _content_length;
        bool                                _complete;
        bool                                _parsed_header;

        bool    _parseRequestLine(const std::string& line);
        bool    _parseHeaders(const std::string& raw);
        bool    _parseBody(const std::string& raw);
        void    _toLowerKeys();

    public:
        Request();
        Request(const Request& copy);
        Request& operator=(const Request& copy);
        ~Request();

        bool    Parse(const std::string& raw);
};
