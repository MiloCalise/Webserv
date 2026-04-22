#include "../../includes/Server/Response.hpp"

Response::Response() : _status_code(200), _status_message("OK") {}

Response::~Response() {}

void Response::setStatus(int code)
{
    _status_code = code;
    _status_message = getStatusMessage(code);
}

void Response::setHeader(const std::string& key, const std::string& value)
{
    _headers[key] = value;
}

void Response::setBody(const std::string& body)
{
    _body = body;
}

std::string Response::build() const
{
    std::ostringstream oss;

    oss << "HTTP/1.1 " << _status_code << " " << _status_message << "\r\n";
    for (std::map<std::string,std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
        oss << it->first << ": " << it->second << "\r\n";
    if (!_body.empty())
        oss << "Content-Length: " << _body.size() << "\r\n";
    oss << "\r\n";
    oss << _body;
    return oss.str();
}

std::string Response::getStatusMessage(int code)
{
    switch (code)
    {
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";
        case 301: return "Moved Permanently";
        case 302: return "Found";
        case 400: return "Bad Request";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 413: return "Payload Too Large";
        case 500: return "Internal Server Error";
        default:  return "Unknown";
    }
}

std::string Response::getMimeType(const std::string& ext)
{
    if (ext == ".html" || ext == ".htm") return "text/html";
    if (ext == ".css")                   return "text/css";
    if (ext == ".js")                    return "application/javascript";
    if (ext == ".json")                  return "application/json";
    if (ext == ".png")                   return "image/png";
    if (ext == ".jpg" || ext == ".jpeg") return "image/jpeg";
    if (ext == ".gif")                   return "image/gif";
    if (ext == ".ico")                   return "image/x-icon";
    return "application/octet-stream";
}
