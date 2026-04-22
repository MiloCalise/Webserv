#include "../includes/Server/Server.hpp"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: ./webserv [configuration file]" << std::endl;
        return 1;
    }

    try
    {
        Config  conf;
        conf.parseConfig(argv[1]);

        Server  server;
        server.startServers(conf);
        server.startLoop();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
