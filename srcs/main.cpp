#include "../includes/Server/Server.hpp"

int main(int argc, char**argv)
{
    if (argc != 2)
        return (std::cerr << "Usage : A single config file\n", 1);
    Config  *config = new Config();
    Server  *server = new Server();
    try
    {
        config->parseConfig(argv[1]);
        server->startServers(*config);
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return (delete(config), delete(server), 1);
    }
    return (delete(config), delete(server), 0);
}
