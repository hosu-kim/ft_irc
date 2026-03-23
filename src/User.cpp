#include "User.hpp"

User::User() : userName(""), nickname(""), fullname(""), hostName(""), registered(false), has_nick(false), has_user(false), pass_ok(false)
{}

User::User(int user_fd) : userName(""), nickname(""), fullname(""), hostName(""), registered(false),
fd(user_fd)
{}


/* ****************** GETTERS **************************** */

std::string User::getUserName() const
{
	return (userName);
}

std::string User::getFullname() const
{
	return (fullname);
}

std::string User::getNickname() const
{
	return (nickname);
}

std::string User::getHostName() const
{
	return (hostName);
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

void User::setName(std::string _userName)
	{userName = _userName;}

void User::setNickname(std::string _nickname) 
	{nickname = _nickname;}

void User::setFullname(std::string _fullname) 
	{fullname = _fullname;}

void User::setHostmask(std::string _hostmask) 
	{hostName = _hostmask;}

void    User::setRegistered(bool flag)
	{registered = flag;}

void    User::setHasUser(bool flag)
	{has_user = flag;}

void    User::setHasNick(bool flag)
	{has_nick = flag;}

void    User::setPassOK(bool flag)
	{pass_ok = flag;}

/* **************** LOGIC FUNCTIONS ************************ */
void User::reply(std::string msg) {
	std::string fullMsg = msg + "\r\n";
	// ssize_t send(int sockfd, const void *buf, size_t len, int flags);
	send(fd, fullMsg.c_str(), fullMsg.size(), 0);
}