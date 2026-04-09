#include "../../includes/Client/Client.hpp"

Client::Client(int fd, ServerConfig *conf) : _fd(fd), _serv(conf), _last_activity(time(NULL)), _header_read(0), _length(0), _bytes_sent(0)
{

}

Client::~Client() {}
