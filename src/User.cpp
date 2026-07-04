#include "User.hpp"
#include "Channel.hpp"
#include <unistd.h>     // close()
#include <sys/socket.h> // send/recv
#include <iostream>     // std::cout

User::User() : user_name_(""), nickname_(""),
			   real_name_(""), host_name_(""),
			   is_registered_(false), has_user_(false),
			   has_nick_(false), pass_ok_(false) {
	last_activity_ = time(NULL);
}

User::User(int user_fd) : fd_(user_fd),
						  user_name_(""), nickname_(""),
						  real_name_(""), host_name_(""),
						  is_registered_(false), has_user_(false),
						  has_nick_(false), pass_ok_(false) {
	last_activity_ = time(NULL);
}


/* GETTERS */

std::string User::getUserName() const { return this->user_name_; }

std::string User::getFullname() const { return this->real_name_; }

std::string User::getNickname() const { return this->nickname_; }

std::string User::getHostName() const { return this->host_name_; }

int User::getFd() const { return this->fd_; }

bool User::isRegistered() const { return this->is_registered_; }

bool User::getHasNick() const { return this->has_nick_; }

bool User::getHasUser() const { return this->has_user_; }

bool User::getPassOK() const { return this->pass_ok_; }

const std::map<std::string, Channel*>& User::getJoinedChannels() const {
	return this->joined_channels_;
}

time_t User::getLastActivity() const { return this->last_activity_; }

/* SETTERS */

void User::setUserName(std::string userName) { this->user_name_ = userName; }

void User::setNickname(std::string nickname) { this->nickname_ = nickname; }

void User::setRealName(std::string realName) { this->real_name_ = realName; }

void User::setHostmask(std::string hostmask) { this->host_name_ = hostmask; }

void User::setRegistered(bool flag) { this->is_registered_ = flag; }

void User::setHasUser(bool flag) { this->has_user_ = flag; }

void User::setHasNick(bool flag) { this->has_nick_ = flag; }

void User::setPassOK(bool flag) { this->pass_ok_ = flag; }

/* METHODS */
void User::reply(std::string msg) {
	std::string fullMsg = msg + "\r\n";
	// ssize_t send(A. socket_fd, B. buf_address, C. len_of_buf, D. flag);
	send(fd_, fullMsg.c_str(), fullMsg.size(), 0);
}

void User::joinChannel(Channel* channel) {
	this->joined_channels_[channel->getChannelName()] = channel;
}

void User::leaveChannel(Channel* channel) {
	this->joined_channels_.erase(channel->getChannelName());
}
void User::updateActivity() {
	this->last_activity_ = time(NULL);
}

