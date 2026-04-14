#include "../../includes/Server/Server.hpp"
#include <cstddef>
#include <string>

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
        size_t  firstline_end = header.find("\r\n");
        if (firstline_end == std::string::npos)
            return false;
        if (!_parseRequestLine(header.substr(0, firstline_end)))
            return false;
        if (!_parseHeaders(header.substr(firstline_end + 2)))
            return false;
        _parsed_header = true;
        if (_headers.count("Content-Length"))
            _content_length = std::stoul(_headers["Content-Length"]);
    }
    else
    {
        size_t body_start = raw.find("\r\n\r\n") + 4;
        std::string body = raw.substr(body_start);
        if (body.size() >= _content_length)
        {
            _body = body.substr(0, _content_length);
            _complete = true;
        }
    }
    return _complete;
}

bool Request::_parseRequestLine(const std::string& line)
{
    std::istringstream ss(line);

    if (!(ss >> _method >> _path >> _version))
        return false;
    if (_method != "GET" && _method != "POST" && _method != "DELETE")
        return false;
    return true;
}

bool Request::_parseHeaders(const std::string& raw)
{
    std::istringstream ss(raw);
    std::string line;

    while (std::getline(ss, line))
    {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);
        if (line.empty())
            continue;
        size_t colon = line.find(':');
        if (colon == std::string::npos)
            return false;
        std::string key = line.substr(0, colon);
        std::string value = line.substr(colon + 1);
        size_t start = value.find_first_not_of(" \t");
        if (start != std::string::npos)
            value = value.substr(start);
        for (size_t i = 0; i < key.size(); i++)
            key[i] = std::tolower(key[i]);
        _headers[key] = value;
    }
    return true;
}
