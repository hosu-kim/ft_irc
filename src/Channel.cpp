#include "Channel.hpp"

/* Orthodox Canonical Form */
Channel::Channel()
	: _channelName("default"), _channelPassword("default"), _channelTopic("default") {}

Channel::Channel(std::string channelName)
	: _channelName(channelName), _channelPassword("default"), _channelTopic("default") {}

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