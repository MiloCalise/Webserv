#pragma once

#include <iostream>
#include <map>
#include <sstream>

class Response
{
    public:
        Response();
        ~Response();

        void        setStatus(int code);
        void        setHeader(const std::string& key, const std::string& value);
        void        setBody(const std::string& body);
        std::string build() const;

        // helpers statiques
        static std::string  getStatusMessage(int code);
        static std::string  getMimeType(const std::string& extension);

    private:
        Response(const Response& copy);
        Response& operator=(const Response& copy);

        int                                 _status_code;
        std::string                         _status_message;
        std::map<std::string, std::string>  _headers;
        std::string                         _body;
};
