#include "../../includes/Server/Server.hpp"
#include <cstddef>
#include <cstdlib>
#include <string>

Request::Request() : _content_length(0), _header_end(0), _complete(false), _parsed_header(false) {}

Request::Request(const Request& copy) : _method(copy._method), _path(copy._path), _version(copy._version), _headers(copy._headers), _body(copy._body), _content_length(copy._content_length), _header_end(copy._header_end), _complete(copy._complete), _parsed_header(copy._parsed_header) {}

Request& Request::operator=(const Request& copy)
{
    if (this == &copy)
        return *this;
    _method         = copy._method;
    _path           = copy._path;
    _version        = copy._version;
    _headers        = copy._headers;
    _body           = copy._body;
    _complete       = copy._complete;
    _parsed_header  = copy._parsed_header;
    _content_length = copy._content_length;
    return *this;
}

Request::~Request() {}

bool Request::Parse(const std::string& raw)
{
    if (!_parsed_header)
    {
        size_t header_end = raw.find("\r\n\r\n");
        size_t skip = 4;
        if (header_end == std::string::npos)
        {
            header_end = raw.find("\n\n");
            skip = 2;
        }
        if (header_end == std::string::npos)
            return false;
        std::string header_section = raw.substr(0, header_end);
        size_t first_line_end = header_section.find("\r\n");
        if (first_line_end == std::string::npos)
            first_line_end = header_section.find("\n");
        if (first_line_end == std::string::npos)
            return false;
        if (!_parseRequestLine(header_section.substr(0, first_line_end)))
            return false;
        size_t headers_start = first_line_end;
        if (raw[headers_start] == '\r') headers_start++;
        headers_start++;
        if (!_parseHeaders(header_section.substr(headers_start)))
            return false;
        _parsed_header = true;
        _header_end = header_end + skip;
        if (_headers.count("content-length"))
            _content_length = std::atol(_headers["content-length"].c_str());
        if (_content_length == 0)
        {
            _complete = true;
            return true;
        }
    }
    if (_parsed_header && !_complete)
    {
        std::string body = raw.substr(_header_end);
        std::cerr << "body_length expected: " << _content_length << std::endl;
        std::cerr << "body received: " << body.size() << std::endl;

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
