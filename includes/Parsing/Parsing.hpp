#pragma once

#include "../Config/Config.hpp"
#include <cstddef>
#include <vector>
#include <string>

class Parsing
{
    private :
        Parsing(const Parsing& copy);
        Parsing& operator=(const Parsing& copy);
        Parsing();

        void    openFile();
    protected :
        size_t                      _pos;

        std::string                 _path;

        std::vector<std::string>    _tokens;

        void                        expect(const std::string& token);
        void                        tokenize(const std::string& content);

        std::string                 peek();
        std::string                 get();

        virtual void                parse() = 0;
        virtual void                parse(ServerConfig& current_serv) = 0;
    public :
        Parsing(const std::string& path);
        virtual ~Parsing();
};
