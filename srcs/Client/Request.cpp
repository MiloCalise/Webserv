#include "../../includes/Server/Server.hpp"
#include <cstddef>

Request::Request() : _content_length(0), _complete(false), _parsed_header(false) {}

Request::~Request() {}

bool    Request::Parse(const std::string& raw)
{
    if (!_parsed_header)
    {
        size_t  header_end = raw.find("\r\n\r\n");
        if (header_end == std::string::npos)
                  return false;
        std::string header = raw.substr(0, header_end);
    }
}