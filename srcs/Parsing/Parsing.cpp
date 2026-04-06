#include "../../includes/Parsing/Parsing.hpp"
#include <fstream>
#include <sstream>
#include <string>

Parsing::Parsing(const std::string& path) : _pos(0), _path(path)
{
    openFile();
}

Parsing::~Parsing() {}

std::string Parsing::peek() {
    if (_pos >= _tokens.size())
        throw std::runtime_error("Error: unexpected end of file");
    return (_tokens[_pos]);
}

std::string Parsing::get() {
    if (_pos >= _tokens.size())
        throw std::runtime_error("Error: unexpected end of file");
    return _tokens[_pos++];
}

void Parsing::expect(const std::string& token) {
    std::string getting = get();
    if (getting != token)
        throw std::runtime_error("Unexpected tokken");
}

void    Parsing::tokenize(const std::string& content)
{
    std::string current;
    bool comment = 0;

        for (size_t i = 0; i < content.size(); i++) {
            char c = content[i];

            if (comment) {
                if (c == '\n')
                    comment = false;
                continue ;
            }

            if (c == '#') {
                comment = true;
                if (!current.empty()) {
                    _tokens.push_back(current);
                    current.clear();
                }
                continue ;
            }

            if (std::isspace(c)) {
                if (!current.empty()) {
                    _tokens.push_back(current);
                    current.clear();
                }
            }
            else if (c == '{' || c == '}' || c == ';') {
                if (!current.empty()) {
                    _tokens.push_back(current);
                    current.clear();
                }
                _tokens.push_back(std::string(1, c));
            }
            else {
                current += c;
            }
        }
        if (!current.empty())
            _tokens.push_back(current);
}

void    Parsing::openFile()
{
    std::ifstream       file(_path.c_str(), std::ifstream::in);
    std::stringstream   buffer;

    if (!file.is_open())
        throw std::runtime_error("Could not open file");
    buffer << file.rdbuf();
    std::string content = buffer.str();
    tokenize(content);
}
