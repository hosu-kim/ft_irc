#include "User.hpp"

User::User() : name(""), nickname(""), fullname(""), hostmask(""), registered(false), has_nick(false), has_user(false), pass_ok(false)
{}

User::User(int user_fd) : name(""), nickname(""), fullname(""), hostmask(""), registered(false),
fd(user_fd)
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

int User::getFd() const {
    return (fd);
}

bool    User::getRegistered()
{
    return (registered);
}

bool    User::getHasNick()
{
    return (has_nick);
}

bool    User::getHasUser()
{
    return (has_user);
}

bool    User::getPassOK()
{
    return (pass_ok);
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

void    User::setRegistered(bool flag)
{
    registered = flag;
}
void    User::setHasUser(bool flag)
{
    has_user = flag;
}
void    User::setHasNick(bool flag)
{
    has_nick = flag;
}
void    User::setPassOK(bool flag)
{
    pass_ok = flag;
}