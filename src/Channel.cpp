#include "../include/Channel.hpp"

/* ======================== Orthodox Canonical Form ========================= */
Channel::Channel()
	: _channelName("default"), _channelPassword("default"),
	 _channelTopic("default"), _userLimit(0) {}

Channel::Channel(std::string channelName, std::string channelPassword, std::string channelTopic, int userLimit)
	: _channelName(channelName), _channelPassword(channelPassword),
	_channelTopic(channelTopic), _userLimit(userLimit) {}

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

/* ============================ Member Functions ============================ */
// RFC standards error code used:
// https://www.rfc-editor.org/rfc/rfc2812.html
// 475(ERR_BADCHANNELKEY): Wrong password for the channel
// 471(ERR_CHANNELISFULL): User limit reached
// 0: Success to join a user
int Channel::joinUser(User* user, std::string password) {
	// 1. if user already exists in the channel
	if (_channelMembers.find(user->getNickname()) != _channelMembers.end())
		return 0; // Success

	// 2. password validation (only when +k[password needed] mode is enabled)
	if (!this->_channelPassword.empty() && this->_channelPassword != password)
		return 475; // ERR_BADCHANNELKEY

	// 3. user limit check (only when +l[num of user limited] mode is enabled)
	// if _userLimit is 0, unlimited user entry allowed
	if (this->_userLimit > 0 && this->_channelMembers.size() >= (size_t)_userLimit)
		return 471;

	// 4. User entry approval
	_channelMembers[user->getNickname()] = user;
	return 0; // Success
}

int Channel::leaveUser(User* user) {
	// 1. user exists in the channel
	std::string nickname = user->getNickname();
	if (_channelMembers.find(nickname) == _channelMembers.end())
		return 442; // ERR_NOTONCHANNEL

	// 2. Deletes user in the channel members
	_channelMembers.erase(nickname);

	// 3. Deletes user in the channel operaters
	_channelOperators.erase(nickname);

	return 0;
}

size_t Channel::getMemberCount() const {
	return _channelMembers.size();
}