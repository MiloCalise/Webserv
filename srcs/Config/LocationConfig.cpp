#include "../../includes/Config/LocationConfig.hpp"

LocationConfig::LocationConfig(std::string path) : _path(path), _autoindex(false) {}

LocationConfig::~LocationConfig() {}

// print la partie location
void LocationConfig::print() const
{
    std::cout << "  Location: " << _path << std::endl;
    if (!_root.empty()) std::cout << "    Root: " << _root << std::endl;
    if (!_index.empty()) std::cout << "    Index: " << _index << std::endl;
    std::cout << "    Autoindex: " << (_autoindex ? "on" : "off") << std::endl;

    if (!_methods.empty())
    {
        std::cout << "    Methods: ";
        for (size_t i = 0; i < _methods.size(); i++)
            std::cout << _methods[i] << (i < _methods.size() - 1 ? " " : "");
        std::cout << std::endl;
    }
}
