#include "Channel.hpp"

/* ======================== Orthodox Canonical Form ========================= */
Channel::Channel()
	: _channelName("default"), _channelPassword(""),
	 _channelTopic(""), _userLimit(0), _isInviteOnly(false),
	 _isTopicRestricted(false), _hasKey(false) {}

Channel::Channel(std::string channelName, std::string channelPassword, std::string channelTopic, int userLimit)
	: _channelName(channelName), _channelPassword(channelPassword),
	_channelTopic(channelTopic), _userLimit(userLimit), _isInviteOnly(false),
	_isTopicRestricted(false) {
		if (channelPassword != "") _hasKey = true;
		else _hasKey = false;
	}

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
		this->_userLimit = src._userLimit;
		this->_channelMembers = src._channelMembers;
		this->_channelOperators = src._channelOperators;
		this->_isInviteOnly = src._isInviteOnly;
		this->_isTopicRestricted = src._isTopicRestricted;
		this->_invitedUsers = src._invitedUsers;
		this->_hasKey = src._hasKey;
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

	// 2. if invite-only mode enabled (+i)
	if (this->_isInviteOnly) {
		if (_invitedUsers.find(user->getNickname()) == _invitedUsers.end())
			return 473;
	}

	// 3. password validation (only when +k[password needed] mode is enabled)
	if (this->_hasKey && this->_channelPassword != password)
		return 475; // ERR_BADCHANNELKEY

	// 4. user limit check (only when +l[num of user limited] mode is enabled)
	// if _userLimit is 0, unlimited user entry allowed
	if (this->_userLimit > 0 && this->_channelMembers.size() >= (size_t)_userLimit)
		return 471;

	// 4. User entry approval and removes user from the invited users
	_channelMembers[user->getNickname()] = user;
	_invitedUsers.erase(user->getNickname());
	return 0; // Success
}

int Channel::removeUser(User* user) {
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

int Channel::addOperator(User* user) {
	if (!user) return -1;

	std::string nickname = user->getNickname();

	// 1. if user is in the channel members.
	if (_channelMembers.find(nickname) == _channelMembers.end())
		return 441; // ERR_USERNOTINCHANNEL

	// 2. if user is already an operator
	if (_channelOperators.find(nickname) != _channelOperators.end())
		return 0;
	
	_channelOperators.insert(nickname);
	return 0;
}

int Channel::removeOperator(User* user) {
	if (!user) return -1;
	std::string nickname = user->getNickname();

	// 1. if the user is a channel member
	if (_channelMembers.find(nickname) == _channelMembers.end()) {
		return 441; // ERR_USERNOTINCHANNEL
	}

	// 2. if the user is an operator
	if (_channelOperators.find(nickname) == _channelOperators.end())
		return 0;

	// 3. remove the user from the operator list
	_channelOperators.erase(nickname);

	return 0;
}

/* Except the sender(exceptUser), all members receive the message*/
void Channel::broadcast(std::string msg, User* exceptUser) {
	if (msg.empty()) return;
	// it's safe to add the IRC standard newline character sequence("\r\n"),
	// if it's not at the end of the message.
	// '\r'(Carriage Return): Sends the cursor to the beginning of the current line
	if (msg.find("\r\n") == std::string::npos)
		msg += "\r\n";

	std::map<std::string, User*>::iterator it;
	for (it = _channelMembers.begin(); it != _channelMembers.end(); ++it) {
		User* targetUser = it->second;

		// If user to exclude is specified and match the current target, skip.
		if (exceptUser != NULL && targetUser == exceptUser)
			continue;

		if (send(targetUser->getFd(), msg.c_str(), msg.size(), 0) == -1)
			std::cerr << "Broadcast send failed to: " << targetUser->getNickname() << std::endl;
	}
}

// ============================= Helper functions for setMode =============================
User* Channel::findUserByNick(std::string nickname) {
	std::map<std::string, User*>::iterator it = _channelMembers.find(nickname);
	if (it == _channelMembers.end())
		return NULL;
	return it->second;
}

bool Channel::stringToInt(const std::string& str, int& result) {
	std::stringstream ss(str);

	if (!(ss >> result) || !ss.eof()) {
		return false;
	}
	return true;
}

// mode: 'k': password needed, 'l': user limit, 'o': operator setup, 't': topic limit
// op: '+'(add/enable), '-'(remove/disable)
// value: password / number of user limit / a specific nickname
int Channel::setMode(char mode, char op, std::string value, User* setter) {
	if (_channelOperators.find(setter->getNickname()) == _channelOperators.end())
		return 482;
	bool isPlus = (op == '+');

	try {
		switch (mode) {
			case 'i':
				_isInviteOnly = isPlus;
				break;
			
			case 't':
				_isTopicRestricted = isPlus;
				break;

			case 'k': // Key(password)
				if (isPlus) {
					if (value.empty()) throw IRCException(461, "ERR_NEEDMOREPARAMS");
					_channelPassword = value;
					_hasKey = true;
				} else {
					_channelPassword = "";
					_hasKey = false;
				}
				break;

			case 'l': // Limit (user limit)
				if (isPlus) {
					if (value.empty()) throw IRCException(461, "ERR_NEEDMOREPARAMS");

					int limit;
					if (!stringToInt(value, limit)) {
						throw IRCException(472, "ERR_UNKNOWNMORE");
					}
					_userLimit = (limit < 0) ? 0 : limit;
				} else {
					_userLimit = 0;
				}
				break;
			
			case 'o':
			{
				if (value.empty()) throw IRCException(461, "ERR_NEEDMOREPARAMS");
				User* target = findUserByNick(value);
				if (!target) throw IRCException(401, "ERR_NOSUCHNICK");

				return isPlus ? this->addOperator(target) : this->removeOperator(target);
			}
			
			default:
				throw IRCException(472, "ERR_UNKNOWNMODE");
		}
	} catch (const IRCException& e) {
		return e.getErrorCode();
	}
		return 0;
}


