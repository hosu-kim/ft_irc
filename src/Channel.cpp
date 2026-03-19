#include "../include/Channel.hpp"

/* Orthodox Canonical Form */
Channel::Channel()
	: _channelName("default"), _channelPassword("default"), _channelTopic("default"), _userLimit(0) {}

Channel::Channel(std::string channelName, std::string channelPassword, std::string channelTopic, int userLimit)
	: _channelName(channelName), _channelPassword(channelPassword), _channelTopic(channelTopic), _userLimit(userLimit) {}

Channel::Channel(const Channel& src) {
	*this = src;
}

Channel& Channel::operator=(const Channel& src) {
	/* REMINDER: Difference between `->` and `.`
	     - `->`: Accessing member variables through pointer
	     - `.`: Direct access to the members of object
	*/
	if (this != &src) {
		this->_channelName = src._channelName;
		this->_channelPassword = src._channelPassword;
		this->_channelTopic = src._channelTopic;
		this->_channelMembers = src._channelMembers;
	}
	return *this;
}

Channel::~Channel() {}
/*==============================================================================*/

int joinUser(User* user, std::string password) {
	if (this->_channelPassword != "" && this->_channelPassword != password)
		return 475;

	if (this->_channelMemebers.size() >= _userLimit) return 471;

	if (_channelMembers.find(user->getNickname()) != _channelMembers.end()) {
		return 0;
	}

	_channelMembers[user->getNickname()] = user;
	return 0; // Success
}