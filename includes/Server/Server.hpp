#pragma once

#include "Response.hpp"
#include "../Client/Client.hpp"
#include "../Socket/Socket.hpp"
#include <set>
#include <sys/stat.h>
#include <string>
#include <vector>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <stdlib.h>

class Server
{
    private:
        // constructeurs dans private pour interdire leur instanciation
        Server(const Server& copy);
        Server& operator=(const Server& copy);
        std::vector<Socket *>               _sockets;
        int                                 _epoll_fd;
        std::set<unsigned int>              _listen_fds;
        std::map<int, Client>               _clients;
        std::map<int, ServerConfig *>       _fd_to_config;

        std::string                         _buildResponse(int fd);
        std::string                         _makeErrorResponse(int code, ServerConfig* config);
        std::string                         _makeRedirectResponse(const std::string& url);
        std::string                         _handleGET(Request& req, LocationConfig* loc, ServerConfig* config);
        std::string                         _handleDELETE(Request& req, LocationConfig* loc, ServerConfig* config);
        std::string                         _handlePOST(Request& req, LocationConfig* loc, ServerConfig* config);
        std::string                         _handleUpload(Request& req, LocationConfig* loc, ServerConfig* config);
        std::string                         _handleMultipart(Request& req, LocationConfig* loc, ServerConfig* config);
        std::string                         _makeAutoindex(const std::string& dirpath, const std::string& urlpath);
        std::string                         _extractFilename(const std::string& headers);
        std::string                         _saveFile(const std::string& content, const std::string& upload_store, const std::string& filename, ServerConfig* config);
        std::string                         _getExtension(const std::string& path);
        std::string                         _handleCGI(Request& req, LocationConfig* loc, ServerConfig* config, const std::string& ext);
        std::string                         _runCGIParent(Request& req, ServerConfig* config, pid_t pid, int pipe_in[2], int pipe_out[2]);
        std::string                         _parseCGIOutput(const std::string& output, ServerConfig* config);
        std::string                         _intToString(int n);

        std::vector<std::string>            _buildCGIEnv(Request& req, ServerConfig* config, const std::string& script);

        LocationConfig                      *_findLocation(ServerConfig *config, const std::string& path);

        void                                _handleAccept(int fd);
        void                                _handleRead(int fd);
        void                                _handleWrite(int fd);
        void                                _handleClose(int fd);
        void                                _checkTimeouts();
        void                                _runCGIChild(Request& req, ServerConfig* config, const std::string& cgi_bin, const std::string& script, int pipe_in[2], int pipe_out[2]);

        bool                                _isMethodAllowed(LocationConfig *, const std::string&);

    public:
        Server();
        ~Server();
        void                                startServers(Config& conf);
        void                                startLoop();
};
