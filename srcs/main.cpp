#include "../includes/Server/Server.hpp"

static Server* g_server = NULL;

void signalHandler(int sig)
{
    (void)sig;
    std::cerr << "\nShutdown signal received" << std::endl;
    if (g_server)
        g_server->cleanup();
}



int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: ./webserv [configuration file]" << std::endl;
        return 1;
    }
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGPIPE, SIG_IGN);
    try
    {
        Config  conf;
        conf.parseConfig(argv[1]);
        conf.printConfig();

        Server  server;
        g_server = &server;
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
