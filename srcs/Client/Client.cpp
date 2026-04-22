#include "../../includes/Client/Client.hpp"

Client::Client() : _fd(-1), _config(NULL), _last_activity(time(NULL)), _bytes_sent(0) {}

Client::Client(int fd, ServerConfig* config) : _fd(fd), _config(config), _last_activity(time(NULL)), _bytes_sent(0) {}

Client::Client(const Client& copy) : _fd(copy._fd), _config(copy._config), _last_activity(copy._last_activity), _read_buffer(copy._read_buffer), _write_buffer(copy._write_buffer), _bytes_sent(copy._bytes_sent), _request(copy._request) {}

Client& Client::operator=(const Client& copy)
{
    if (this == &copy)
        return *this;
    _fd            = copy._fd;
    _config        = copy._config;
    _read_buffer   = copy._read_buffer;
    _request       = copy._request;
    _write_buffer  = copy._write_buffer;
    _bytes_sent    = copy._bytes_sent;
    _last_activity = copy._last_activity;
    return *this;
}

Client::~Client() {}

void Client::appendToReadBuffer(const std::string& data)
{
    _read_buffer += data;
    _last_activity = time(NULL);
}

bool Client::tryParseRequest()
{
    return _request.Parse(_read_buffer);
}

bool Client::isRequestComplete() const
{
    return _request._complete;
}

bool Client::hasDataToSend() const
{
    return _bytes_sent < _write_buffer.size();
}

void Client::setWriteBuffer(const std::string& response)
{
    _write_buffer = response;
    _bytes_sent = 0;
}

ssize_t Client::sendChunk()
{
    size_t  remaining = _write_buffer.size() - _bytes_sent;
    ssize_t sent = send(_fd, _write_buffer.c_str() + _bytes_sent, remaining, 0);
    if (sent > 0)
        _bytes_sent += sent;
    return sent;
}
