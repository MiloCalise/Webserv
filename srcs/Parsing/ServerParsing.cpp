#include "../../includes/Parsing/ServerParsing.hpp"
#include <cstdlib>
#include <stdexcept>

ServerParsing::ServerParsing(const std::string& path) : LocationParsing(path)
{
    parse();
}

ServerParsing::~ServerParsing() {}

// parsing pour la partie server du conf
void    ServerParsing::parse()
{
    while (_pos < _tokens.size())
    {
        std::string token = get();
        if (token == "server")
        {
            ServerConfig current_server;
            expect("{");
            while (peek() != "}") {
                std::string directive = get();
        if (directive == "listen")
        {
            current_server._port = std::atoi(get().c_str());
            expect(";");
        }
        else if (directive == "server_name")
        {
            current_server._server_name = get();
            expect(";");
        }
        else if (directive == "error_page")
        {
            int code = std::atoi(get().c_str());
            std::string page = get();
            current_server._error_pages[code] = page;
            expect(";");
        }
        else if (directive == "client_max_body_size")
        {
            current_server._client_max_body = std::atoi(get().c_str());
            expect(";");
        }
        else if (directive == "location")
        {
            LocationParsing::parse(current_server);
        }
        else
        {
            throw std::runtime_error("Error: unknown directive");
        }
            }
            expect("}");
            this->_servers.push_back(current_server);
        }
        else
        {
            throw std::runtime_error("Error: expected 'server' block");
        }
    }
}

const std::vector<ServerConfig>& ServerParsing::getServers() const
{
    return this->_servers;
}

void ServerParsing::parse(ServerConfig& current_serv)
{
    (void)current_serv;
}
