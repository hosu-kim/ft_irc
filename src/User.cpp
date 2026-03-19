#include "User.hpp"

User::User() : name(""), nickname(""), fullname(""), hostmask(""), registered(false)
{}


/* ****************** GETTERS **************************** */

std::string User::getName() const
{
    return (name);
}

std::string User::getFullname() const
{
    return (fullname);
}

std::string User::getNickname() const
{
    return (nickname);
}

std::string User::getHostmask() const
{
    return (hostmask);
}

/* **************** SETTERS ************************ */

void User::setName(std::string _name)
{
    name = _name;
}

void User::setNickname(std::string _nickname) 
{
    nickname = _nickname;
}

void User::setFullname(std::string _fullname) 
{
    fullname = _fullname;
}

void User::setHostmask(std::string _hostmask) 
{
    hostmask = _hostmask;
}