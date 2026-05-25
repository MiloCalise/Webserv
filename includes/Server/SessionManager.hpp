#pragma once

#include <ctime>
#include <map>
#include <string>

class SessionManager
{
    private:
        struct Session
        {
            std::string user;
            std::time_t expires_at;
        };

        std::map<std::string, Session> _sessions;

        std::string _generateSessionId() const;
        void        _cleanupExpired();

    public:
        SessionManager();

        std::string createSession(const std::string& user);
        bool        isValid(const std::string& session_id);
        void        destroySession(const std::string& session_id);
        std::string getUser(const std::string& session_id) const;
};