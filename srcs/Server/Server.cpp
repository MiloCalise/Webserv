#include "../../includes/Server/Server.hpp"
#include <sys/epoll.h>

Server::Server() : _epoll_fd(-1), _isRunning(false) {}

Server::~Server()
{
    for (size_t i = 0; i < _sockets.size(); i++)
        delete _sockets[i];
    if (_epoll_fd != -1)
        close(_epoll_fd);
}

void    Server::startServers(Config& conf)
{
    for (unsigned long i = 0; i < conf._servers.size(); i++)
    {
        Socket  *s = new Socket(conf._servers[i]);
        _listen_fds.insert(s->_sock);
        _fd_to_config[s->_sock] = &conf._servers[i];
        _sockets.push_back(s);
    }
    // argument de create n'a pas d'importance
    _epoll_fd = epoll_create(1);
    if (_epoll_fd == -1)
        throw std::runtime_error("Could not create epoll instance");
    for (unsigned long i = 0; i < conf._servers.size(); i++)
    {
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = _sockets[i]->_sock;
        if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _sockets[i]->_sock, &ev) == -1)
            throw std::runtime_error("Could not add socket to epoll instance");
    }
}

void    Server::startLoop()
{
    struct epoll_event events[64];
    _isRunning = true;
    while (1)
    {
        int nb = epoll_wait(_epoll_fd, events, 64, -1);
        if (nb == -1)
        {
            if (errno == EINTR)
                return ;
            throw std::runtime_error("Error in epoll wait");
        }
        for (int i = 0; i < nb; i++)
        {
            int fd = events[i].data.fd;
            if (_listen_fds.count(fd))
                _handleAccept(fd);
            else if (events[i].events & EPOLLIN)
                _handleRead(fd);
            else if (events[i].events & EPOLLOUT)
                _handleWrite(fd);
            else if (events[i].events & (EPOLLHUP | EPOLLERR))
                _handleClose(fd);
        }
        _checkTimeouts();
    }
    cleanup();
}

void Server::cleanup()
{
    for (std::map<int,Client>::iterator it = _clients.begin();
         it != _clients.end(); ++it)
    {
        epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, it->first, NULL);
        close(it->first);
    }
    _clients.clear();
    for (size_t i = 0; i < _sockets.size(); i++)
    {
        epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, _sockets[i]->_sock, NULL);
        delete _sockets[i];
    }
    _sockets.clear();
    if (_epoll_fd != -1)
    {
        close(_epoll_fd);
        _epoll_fd = -1;
    }
    std::cerr << "Server stopped cleanly" << std::endl;
}

void Server::_checkTimeouts()
{
    time_t  now     = time(NULL);
    int     timeout = 30;
    std::vector<int> to_close;

    for (std::map<int,Client>::iterator it = _clients.begin();
         it != _clients.end(); ++it)
    {
        if (now - it->second._last_activity > timeout)
            to_close.push_back(it->first);
    }
    for (size_t i = 0; i < to_close.size(); i++)
    {
        std::cerr << "Client " << to_close[i] << " timed out" << std::endl;
        _handleClose(to_close[i]);
    }
}

void    Server::_handleAccept(int fd)
{
    int client_fd = accept(fd, NULL, NULL);
    if (client_fd == -1)
        return ;
    fcntl(client_fd, F_SETFL, O_NONBLOCK);
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = client_fd;
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1)
    {
        close(client_fd);
        return;
    }
    _clients[client_fd] = Client(client_fd, _fd_to_config[fd]);
    _fd_to_config[client_fd] = _fd_to_config[fd];
}

void    Server::_handleRead(int fd)
{
    char    buf[4096];
    ssize_t n = recv(fd, buf, sizeof(buf), 0);

    if (n <= 0)
    {
        std::cerr << "Client " << fd << " disconnected" << std::endl;
        _handleClose(fd);
        return;
    }
    _clients[fd].appendToReadBuffer(std::string(buf, n));
    if (_clients[fd].tryParseRequest())
    {
        std::string response = _buildResponse(fd);
        _clients[fd].setWriteBuffer(response);
        struct epoll_event ev;
        ev.events = EPOLLOUT;
        ev.data.fd = fd;
        epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, fd, &ev);
    }
    std::cerr << "buffer size: " << _clients[fd]._read_buffer.size() << std::endl;
    std::cerr << "request complete: " << _clients[fd].tryParseRequest() << std::endl;
}

void    Server::_handleWrite(int fd)
{
    ssize_t sent = _clients[fd].sendChunk();
    if (sent == -1 || !_clients[fd].hasDataToSend())
        _handleClose(fd);
}

void    Server::_handleClose(int fd)
{
    epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, NULL);
    close(fd);
    _clients.erase(fd);
    _fd_to_config.erase(fd);
}

std::string Server::_buildResponse(int fd)
{
    Request&        req    = _clients[fd]._request;
    ServerConfig*   config = _fd_to_config[fd];
    LocationConfig* loc    = _findLocation(config, req._path);
    if (!loc)
        return _makeErrorResponse(404, config);
    if (!_isMethodAllowed(loc, req._method))
        return _makeErrorResponse(405, config);
    if (req._body.size() > config->_client_max_body)
        return _makeErrorResponse(413, config);
    if (!loc->_redirect.empty())
        return _makeRedirectResponse(loc->_redirect);
    std::string ext = _getExtension(req._path);
    if (ext.empty() && !loc->_index.empty())
        ext = _getExtension(loc->_index);
    if (loc->_cgi.count(ext))
        return _handleCGI(req, loc, config, ext);
    if (req._method == "GET")
        return _handleGET(req, loc, config);
    if (req._method == "POST")
        return _handlePOST(req, loc, config);
    if (req._method == "DELETE")
        return _handleDELETE(req, loc, config);
    return _makeErrorResponse(501, config);
}

std::string Server::_getExtension(const std::string& path)
{
    std::string clean = path;
    size_t qpos = clean.find('?');
    if (qpos != std::string::npos)
        clean = clean.substr(0, qpos);
    size_t dot = clean.rfind('.');
    if (dot == std::string::npos)
        return "";
    return clean.substr(dot);
}

std::string Server::_handleCGI(Request& req, LocationConfig* loc, ServerConfig* config, const std::string& ext)
{
    std::string cgi_bin = loc->_cgi.at(ext);

    char cwd[4096];
    if (!getcwd(cwd, sizeof(cwd)))
        return _makeErrorResponse(500, config);
    std::string base = std::string(cwd);

    std::string rel_path = req._path;
    if (rel_path.substr(0, loc->_path.size()) == loc->_path)
        rel_path = rel_path.substr(loc->_path.size());
    if (!rel_path.empty() && rel_path[0] == '/')
        rel_path = rel_path.substr(1);
    if (rel_path.empty() && !loc->_index.empty())
        rel_path = loc->_index;

    std::string root = loc->_root.empty() ? config->_root : loc->_root;
    if (!root.empty() && root[0] == '.')
        root = base + "/" + root.substr(root[1] == '/' ? 2 : 1);

    std::string script = root + "/" + rel_path;
    std::cerr << "CGI script path absolute: " << script << std::endl;

    struct stat st;
    if (stat(script.c_str(), &st) == -1)
        return _makeErrorResponse(404, config);
    int pipe_in[2];
    int pipe_out[2];
    if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1)
        return _makeErrorResponse(500, config);
    pid_t pid = fork();
    if (pid == -1)
    {
        close(pipe_in[0]);  close(pipe_in[1]);
        close(pipe_out[0]); close(pipe_out[1]);
        return _makeErrorResponse(500, config);
    }
    if (pid == 0)
        _runCGIChild(req, config, cgi_bin, script, pipe_in, pipe_out);
    return _runCGIParent(req, config, pid, pipe_in, pipe_out);
}

void Server::_runCGIChild(Request& req, ServerConfig* config, const std::string& cgi_bin, const std::string& script, int pipe_in[2], int pipe_out[2])
{
    dup2(pipe_in[0], STDIN_FILENO);
    close(pipe_in[0]);
    close(pipe_in[1]);
    dup2(pipe_out[1], STDOUT_FILENO);
    close(pipe_out[0]);
    close(pipe_out[1]);
    if (_epoll_fd != -1)
            close(_epoll_fd);
    for (size_t i = 0; i < _sockets.size(); i++)
            close(_sockets[i]->_sock);
    for (std::map<int,Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        close(it->first);
    std::vector<std::string> env_strings = _buildCGIEnv(req, config, script);
    std::vector<char*> env;
    for (size_t i = 0; i < env_strings.size(); i++)
        env.push_back(const_cast<char*>(env_strings[i].c_str()));
    env.push_back(NULL);
    char* args[3];
    args[0] = const_cast<char*>(cgi_bin.c_str());
    args[1] = const_cast<char*>(script.c_str());
    args[2] = NULL;
    std::string script_dir = script.substr(0, script.rfind('/'));
    chdir(script_dir.c_str());
    execve(cgi_bin.c_str(), args, &env[0]);
    exit(1);
}

std::vector<std::string> Server::_buildCGIEnv(Request& req, ServerConfig* config, const std::string& script)
{
    std::vector<std::string> env;
    std::string path     = req._path;
    std::string query    = "";
    size_t qpos = path.find('?');

    if (qpos != std::string::npos)
    {
        query = path.substr(qpos + 1);
        path  = path.substr(0, qpos);
    }
    env.push_back("REQUEST_METHOD="   + req._method);
    env.push_back("QUERY_STRING="     + query);
    env.push_back("PATH_INFO="        + path);
    env.push_back("SCRIPT_FILENAME="  + script);
    env.push_back("CONTENT_TYPE="     + req._headers["content-type"]);
    env.push_back("SERVER_PORT="      + _intToString(config->_port));
    env.push_back("SERVER_NAME="      + config->_server_name);
    env.push_back("GATEWAY_INTERFACE=CGI/1.1");
    env.push_back("SERVER_PROTOCOL=HTTP/1.1");
    if (!req._body.empty())
    {
        env.push_back("CONTENT_LENGTH=" + _intToString(req._body.size()));
    }
    return env;
}

std::string Server::_runCGIParent(Request& req, ServerConfig* config, pid_t pid, int pipe_in[2], int pipe_out[2])
{
    std::string cgi_output;
    char        buf[4096];
    ssize_t     n;
    int         status;
    pid_t       result;

    close(pipe_in[0]);
    close(pipe_out[1]);
    if (!req._body.empty())
    {
        write(pipe_in[1], req._body.c_str(), req._body.size());
    }
    close(pipe_in[1]);
    time_t start = time(NULL);
    while (1)
    {
        result = waitpid(pid, &status, WNOHANG);
        if (result == pid)
            break;
        if (time(NULL) - start > 5)
        {
            kill(pid, SIGKILL);
            waitpid(pid, &status, 0);
            close(pipe_out[0]);
            return _makeErrorResponse(504, config);
        }
        usleep(10000);
    }
    while ((n = read(pipe_out[0], buf, sizeof(buf))) > 0)
        cgi_output += std::string(buf, n);
    close(pipe_out[0]);
    std::cerr << "CGI output: [" << cgi_output << "]" << std::endl;
    std::cerr << "CGI output size: " << cgi_output.size() << std::endl;
    return _parseCGIOutput(cgi_output, config);
}

std::string Server::_parseCGIOutput(const std::string& output, ServerConfig* config)
{
    if (output.empty())
        return _makeErrorResponse(500, config);
    size_t sep = output.find("\r\n\r\n");
    size_t skip = 4;
    if (sep == std::string::npos)
    {
        sep = output.find("\n\n");
        skip = 2;
    }
    if (sep == std::string::npos)
        return _makeErrorResponse(500, config);
    std::string cgi_headers = output.substr(0, sep);
    std::string body        = output.substr(sep + skip);
    Response resp;
    resp.setStatus(200);
    std::istringstream ss(cgi_headers);
    std::string line;
    while (std::getline(ss, line))
    {
        if (!line.empty() && line[line.size()-1] == '\r')
            line.erase(line.size()-1);
        size_t colon = line.find(':');
        if (colon == std::string::npos)
            continue;
        std::string key   = line.substr(0, colon);
        std::string value = line.substr(colon + 2);
        if (key == "Status")
            resp.setStatus(std::atoi(value.c_str()));
        else
            resp.setHeader(key, value);
    }
    resp.setBody(body);
    return resp.build();
}

LocationConfig* Server::_findLocation(ServerConfig* config, const std::string& path)
{
    LocationConfig* best = NULL;
    size_t          best_len = 0;

    for (size_t i = 0; i < config->_locations.size(); i++)
    {
        const std::string& loc_path = config->_locations[i]._path;
        if (path.substr(0, loc_path.size()) == loc_path)
        {
            if (loc_path.size() > best_len)
            {
                best_len = loc_path.size();
                best = &config->_locations[i];
            }
        }
    }
    return best;
}

std::string Server::_makeErrorResponse(int code, ServerConfig* config)
{
    Response resp;
    resp.setStatus(code);
    resp.setHeader("Content-Type", "text/html");

    if (config->_error_pages.count(code))
    {
        std::string filepath = config->_root + config->_error_pages[code];
        std::ifstream file(filepath.c_str());
        if (file.is_open())
        {
            std::ostringstream ss;
            ss << file.rdbuf();
            resp.setBody(ss.str());
            return resp.build();
        }
    }
    std::ostringstream body;
    body << "<html><body><h1>"
         << code << " " << Response::getStatusMessage(code)
         << "</h1></body></html>";
    resp.setBody(body.str());
    return resp.build();
}

bool    Server::_isMethodAllowed(LocationConfig *loc, const std::string& method)
{
    if (!loc)
        return false;
    if (loc->_methods.empty())
            return true;
    for (size_t i = 0; i < loc->_methods.size(); i++)
    {
        if (loc->_methods[i] == method)
            return true;
    }
    return false;
}

std::string Server::_makeRedirectResponse(const std::string& url)
{
    Response resp;
    resp.setStatus(302);
    resp.setHeader("Location", url);
    resp.setBody("<html><body><h1>302 Found</h1></body></html>");
    return resp.build();
}

std::string Server::_makeAutoindex(const std::string& dirpath, const std::string& urlpath)
{
    DIR* dir = opendir(dirpath.c_str());
    if (!dir)
        return _makeErrorResponse(403, NULL);
    std::ostringstream body;
    body << "<html><head><title>Index of " << urlpath << "</title></head><body>";
    body << "<h1>Index of " << urlpath << "</h1><hr><pre>";
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        std::string name = entry->d_name;
        std::string href = urlpath;
        if (href[href.size() - 1] != '/')
            href += '/';
        href += name;
        if (entry->d_type == DT_DIR)
            name += '/';
        body << "<a href=\"" << href << "\">" << name << "</a>\n";
    }
    closedir(dir);
    body << "</pre><hr></body></html>";
    Response resp;
    resp.setStatus(200);
    resp.setHeader("Content-Type", "text/html");
    resp.setBody(body.str());
    return resp.build();
}

std::string Server::_handleGET(Request& req, LocationConfig* loc, ServerConfig* config)
{
    std::string root = loc->_root.empty() ? config->_root : loc->_root;

    std::string rel_path = req._path;
    if (rel_path.substr(0, loc->_path.size()) == loc->_path)
        rel_path = rel_path.substr(loc->_path.size());
    if (!rel_path.empty() && rel_path[0] == '/')
        rel_path = rel_path.substr(1);
    std::string filepath = root + '/' + rel_path;
    std::cerr << "GET filepath: " << filepath << std::endl;
    if (filepath[filepath.size() - 1] == '/')
    {
        if (!loc->_index.empty())
            filepath += loc->_index;
        else if (loc->_autoindex)
            return _makeAutoindex(filepath, req._path);
        else
            return _makeErrorResponse(403, config);
    }
    struct stat st;
    if (stat(filepath.c_str(), &st) == -1)
        return _makeErrorResponse(404, config);
    if (S_ISDIR(st.st_mode))
    {
        if (loc->_autoindex)
            return _makeAutoindex(filepath, req._path);
        return _makeErrorResponse(403, config);
    }
    std::ifstream file(filepath.c_str(), std::ios::binary);
    if (!file.is_open())
        return _makeErrorResponse(403, config);
    std::ostringstream ss;
    ss << file.rdbuf();
    std::string body = ss.str();
    size_t dot = filepath.rfind('.');
    std::string ext = (dot != std::string::npos) ? filepath.substr(dot) : "";
    Response resp;
    resp.setStatus(200);
    resp.setHeader("Content-Type", Response::getMimeType(ext));
    resp.setBody(body);
    return resp.build();
}

std::string Server::_handleDELETE(Request& req, LocationConfig* loc, ServerConfig* config)
{
    std::string root     = loc->_root.empty() ? config->_root : loc->_root;
    std::string rel_path = req._path;
    if (rel_path.substr(0, loc->_path.size()) == loc->_path)
        rel_path = rel_path.substr(loc->_path.size());
    if (!rel_path.empty() && rel_path[0] == '/')
        rel_path = rel_path.substr(1);
    std::string filepath = root + '/' + rel_path;
    std::cerr << "DELETE filepath: " << filepath << std::endl;
    struct stat st;
    if (stat(filepath.c_str(), &st) == -1)
        return _makeErrorResponse(404, config);
    if (S_ISDIR(st.st_mode))
        return _makeErrorResponse(403, config);
    if (remove(filepath.c_str()) == -1)
        return _makeErrorResponse(403, config);
    Response resp;
    resp.setStatus(204);
    return resp.build();
}

std::string Server::_handlePOST(Request& req, LocationConfig* loc, ServerConfig* config)
{
    std::string ext = _getExtension(req._path);
    if (ext.empty() && !loc->_index.empty())
        ext = _getExtension(loc->_index);
    if (!ext.empty() && loc->_cgi.count(ext))
        return _handleCGI(req, loc, config, ext);
    if (!loc->_upload.empty())
        return _handleUpload(req, loc, config);
    return _makeErrorResponse(405, config);
}

std::string Server::_handleUpload(Request& req, LocationConfig* loc, ServerConfig* config)
{
    std::string content_type = req._headers["content-type"];
    std::string body         = req._body;

    std::cerr << "upload body size: " << req._body.size() << std::endl;
    std::cerr << "max body size: " << config->_client_max_body << std::endl;
    std::cerr << "content-type: " << req._headers["content-type"] << std::endl;
    if (body.empty())
        return _makeErrorResponse(400, config);
    struct stat st;
    if (stat(loc->_upload.c_str(), &st) == -1 || !S_ISDIR(st.st_mode))
        return _makeErrorResponse(500, config);
    if (content_type.find("multipart/form-data") != std::string::npos)
        return _handleMultipart(req, loc, config);
    return _saveFile(body, loc->_upload, req._path, config);
}

std::string Server::_handleMultipart(Request& req, LocationConfig* loc, ServerConfig* config)
{
    std::string content_type = req._headers["content-type"];
    size_t bpos = content_type.find("boundary=");

    if (bpos == std::string::npos)
        return _makeErrorResponse(400, config);
    std::string boundary = "--" + content_type.substr(bpos + 9);
    std::string body = req._body;
    size_t part_start = body.find(boundary);
    if (part_start == std::string::npos)
        return _makeErrorResponse(400, config);
    part_start += boundary.size() + 2;
    size_t header_end = body.find("\r\n\r\n", part_start);
    if (header_end == std::string::npos)
        return _makeErrorResponse(400, config);
    std::string part_headers = body.substr(part_start, header_end - part_start);
    std::string filename = _extractFilename(part_headers);
    if (filename.empty())
        return _makeErrorResponse(400, config);
    size_t content_start = header_end + 4;
    size_t content_end   = body.find("\r\n" + boundary, content_start);
    if (content_end == std::string::npos)
        return _makeErrorResponse(400, config);
    std::string file_content = body.substr(content_start, content_end - content_start);
    return _saveFile(file_content, loc->_upload, filename, config);
}

std::string Server::_extractFilename(const std::string& headers)
{
    size_t pos = headers.find("filename=\"");
    if (pos == std::string::npos)
        return "";
    pos += 10;
    size_t end = headers.find("\"", pos);
    if (end == std::string::npos)
        return "";
    return headers.substr(pos, end - pos);
}

std::string Server::_saveFile(const std::string& content, const std::string& upload_store, const std::string& filename, ServerConfig* config)
{
    std::string clean_name = filename;
    size_t slash = clean_name.rfind('/');
    if (slash != std::string::npos)
        clean_name = clean_name.substr(slash + 1);
    std::string filepath = upload_store + "/" + clean_name;
    std::ofstream file(filepath.c_str(), std::ios::binary);
    if (!file.is_open())
        return _makeErrorResponse(500, config);
    file.write(content.c_str(), content.size());
    if (file.fail())
        return _makeErrorResponse(500, config);
    file.close();
    Response resp;
    resp.setStatus(201);
    resp.setHeader("Content-Type", "text/html");
    resp.setBody("<html><body><p>File uploaded successfully.</p></body></html>");
    return resp.build();
}

std::string Server::_intToString(int n)
{
    std::ostringstream oss;
    oss << n;
    return oss.str();
}
