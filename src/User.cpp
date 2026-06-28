#include "User.hpp"
#include "Channel.hpp"

User::User() : _userName(""), _nickname(""),
			   _realName(""), _hostName(""),
			   registered(false), _hasUser(false),
			   _hasNick(false), _passOk(false) {
	_lastActivity = time(NULL);
}

User::User(int user_fd) : _fd(user_fd),
						  _userName(""), _nickname(""),
						  _realName(""), _hostName(""),
						  registered(false), _hasUser(false),
						  _hasNick(false), _passOk(false) {
	_lastActivity = time(NULL);
}


/* ****************** GETTERS **************************** */

std::string User::getUserName() const
{
	return (_userName);
}

std::string User::getFullname() const
{
	return (_realName);
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
	return (_fd);
}

bool    User::getRegistered()
{
	return (registered);
}

bool    User::getHasNick()
{
	return (_hasNick);
}

bool    User::getHasUser()
{
	return (_hasUser);
}

bool    User::getPassOK()
{
	return (_passOk);
}

const std::map<std::string, Channel*>& User::getJoinedChannels() const {
	return this->_joinedChannels;
}

/* **************** SETTERS ************************ */

void User::setUserName(std::string userName)
	{this->_userName = userName;}

void User::setNickname(std::string nickname) 
	{this->_nickname = nickname;}

void User::setRealName(std::string realName) 
	{this->_realName = realName;}

void User::setHostmask(std::string hostmask) 
	{this->_hostName = hostmask;}

void    User::setRegistered(bool flag)
	{this->registered = flag;}

void    User::setHasUser(bool flag)
	{this->_hasUser = flag;}

void    User::setHasNick(bool flag)
	{this->_hasNick = flag;}

void    User::setPassOK(bool flag)
	{this->_passOk = flag;}

/* **************** LOGIC FUNCTIONS ************************ */
void User::reply(std::string msg) {
	std::string fullMsg = msg + "\r\n";
	// ssize_t send(int sockfd, const void *buf, size_t len, int flags);
	send(_fd, fullMsg.c_str(), fullMsg.size(), 0);
}

void User::joinChannel(Channel* channel) {
	this->_joinedChannels[channel->getChannelName()] = channel;
}

void User::leaveChannel(Channel* channel) {
	this->_joinedChannels.erase(channel->getChannelName());
}
void User::updateActivity() {
	_lastActivity = time(NULL);
}

time_t User::getLastActivity() const {
	return (_lastActivity);
}
