#include "User.hpp"
#include "Channel.hpp"

User::User() : _userName(""), _nickname(""), _fullname(""), _hostName(""), registered(false), has_nick(false), has_user(false), pass_ok(false)
{}

User::User(int user_fd) : _userName(""), _nickname(""), _fullname(""), _hostName(""), registered(false),
fd(user_fd)
{}


/* ****************** GETTERS **************************** */

std::string User::getUserName() const
{
	return (_userName);
}

std::string User::getFullname() const
{
	return (_fullname);
}

std::string User::getNickname() const
{
	return (_nickname);
}

std::string User::getHostName() const
{
	return (_hostName);
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

const std::map<std::string, Channel*>& User::getJoinedChannels() const {
	return this->_joinedChannels;
}

/* **************** SETTERS ************************ */

void User::setName(std::string _userName)
	{_userName = _userName;}

void User::setNickname(std::string _nickname) 
	{_nickname = _nickname;}

void User::setFullname(std::string _fullname) 
	{_fullname = _fullname;}

void User::setHostmask(std::string _hostmask) 
	{_hostName = _hostmask;}

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

void User::joinChannel(Channel* channel) {
	this->_joinedChannels[channel->getChannelName()] = channel;
}

void User::removeChannel(Channel* channel) {
	this->_joinedChannels.erase(channel->getChannelName());
}