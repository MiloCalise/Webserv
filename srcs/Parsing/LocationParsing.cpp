#include "../../includes/Parsing/LocationParsing.hpp"
#include <stdexcept>

LocationParsing::LocationParsing(const std::string& path) : Parsing(path) {}

LocationParsing::~LocationParsing() {}

void LocationParsing::parse() {}

// parsing de la partie location du conf
void LocationParsing::parse(ServerConfig& current_server)
{
    std::string loc_path = get();
    LocationConfig current_location(loc_path);

    expect("{");

    while (peek() != "}")
    {
        std::string directive = get();

        if (directive == "root")
        {
            current_location._root = get();
            expect(";");
        }
        else if (directive == "index")
        {
            current_location._index = get();
            expect(";");
        }
        else if (directive == "upload_store")
        {
            current_location._upload = get();
            expect(";");
        }
        else if (directive == "cgi_ext")
        {
            std::string ext = get();
            std::string bin = get();
            current_location._cgi[ext] = bin;
            expect(";");
        }
        else if (directive == "return")
        {
            current_location._redirect = get();
            expect(";");
        }
        else if (directive == "autoindex")
        {
            std::string autoindex_value = get();
            current_location._autoindex = (autoindex_value == "on" || autoindex_value == "true");
            expect(";");
        }
        else if (directive == "allow_methods")
        {
            while (peek() != ";")
                current_location._methods.push_back(get());
            expect(";");
        }
        else
        {
            throw std::runtime_error("Error: unknown directive in location block");
        }
    }
    current_server.addLocation(current_location);
    expect("}");
}
