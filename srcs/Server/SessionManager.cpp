#include "../../includes/Server/SessionManager.hpp"

#include <cstdlib>
#include <iomanip>
#include <sstream>

SessionManager::SessionManager()
{
    std::srand(static_cast<unsigned int>(std::time(NULL)));
}

std::string SessionManager::_generateSessionId() const
{
    std::ostringstream oss;

    for (int i = 0; i < 4; ++i)
    {
        unsigned int part = (static_cast<unsigned int>(std::rand()) << 16) ^ static_cast<unsigned int>(std::rand());
        oss << std::hex << std::setw(8) << std::setfill('0') << part;
    }
    return oss.str();
}

void SessionManager::_cleanupExpired()
{
    std::time_t now = std::time(NULL);
    std::map<std::string, Session>::iterator it = _sessions.begin();

    while (it != _sessions.end())
    {
        if (it->second.expires_at <= now)
            _sessions.erase(it++);
        else
            ++it;
    }
}

std::string SessionManager::createSession(const std::string& user)
{
    _cleanupExpired();

    Session session;
    session.user = user;
    session.expires_at = std::time(NULL) + 3600;

    std::string session_id = _generateSessionId();
    _sessions[session_id] = session;
    return session_id;
}

bool SessionManager::isValid(const std::string& session_id)
{
    _cleanupExpired();
    return _sessions.find(session_id) != _sessions.end();
}

void SessionManager::destroySession(const std::string& session_id)
{
    std::map<std::string, Session>::iterator it = _sessions.find(session_id);
    if (it != _sessions.end())
        _sessions.erase(it);
}

std::string SessionManager::getUser(const std::string& session_id) const
{
    std::map<std::string, Session>::const_iterator it = _sessions.find(session_id);
    if (it == _sessions.end())
        return "";
    if (it->second.expires_at <= std::time(NULL))
        return "";
    return it->second.user;
}