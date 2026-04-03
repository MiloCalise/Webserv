#include "../includes/config.hpp"
#include <sys/epoll.h>

int main(int argc, char**argv)
{
    if (argc != 2)
        return (std::cerr << "Usage : A single config file\n", 1);
    Config  *config = new Config();
    try
    {
        config->parseConfig(argv[1]);
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return (delete(config), 1);
    }
    return (delete(config), 0);
}
