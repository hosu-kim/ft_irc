#include "Channel.hpp"
#include "User.hpp"
#include <iostream>
#include <sys/socket.h>
#include <sstream>

/* ======================== Orthodox Canonical Form ========================= */

Channel::Channel(const std::string& channelName, User* channelOperator)
	: channel_name_(channelName), channel_password_(""),
	  is_invite_only_(false), is_topic_restricted_(false),
	  has_key_(false), member_limit_(0) {
	this->channel_members_[channelOperator->getNickname()] = channelOperator;
	this->channel_operators_.insert(channelOperator->getNickname());
	channelOperator->joinChannel(this);
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
		this->channel_name_ = src.channel_name_;
		this->channel_password_ = src.channel_password_;
		this->channel_topic_ = src.channel_topic_;
		this->member_limit_ = src.member_limit_;
		this->channel_members_ = src.channel_members_;
		this->channel_operators_ = src.channel_operators_;
		this->is_invite_only_ = src.is_invite_only_;
		this->is_topic_restricted_ = src.is_topic_restricted_;
		this->invited_users_ = src.invited_users_;
		this->has_key_ = src.has_key_;
	}
	return *this;
}

Channel::~Channel() {}

/* GETTERS */

// Return the num of members in the channel
size_t Channel::getMemberCount() const {
	return this->channel_members_.size();
}

// If the channel has modes ('i', 't', 'k', 'l'), return true,
// if not, return false
bool Channel::hasMode(char mode) const {

	// Switch-case is more efficient and readable than if-else for single-value conditions
	switch(mode) {
		case 'i': return this->is_invite_only_;
		case 't': return this->is_topic_restricted_;
		case 'k': return !this->channel_password_.empty();
		case 'l': return this->member_limit_ > 0;
		default: return false;
	}
}

std::string Channel::getChannelKey() const { return this->channel_password_; }

size_t Channel::getMemberLimit() const { return this->member_limit_; }

std::string Channel::getChannelName() const { return this->channel_name_; }

size_t Channel::getUserCount() const { return this->channel_members_.size(); }

std::string Channel::getChannelTopic() const { return this->channel_topic_; }

const std::map<std::string, User*>& Channel::getMembers() const { return channel_members_; }

bool Channel::isUserInChannel(std::string nickname) const {
	if (this->channel_members_.find(nickname) != this->channel_members_.end())
		return true;
	return false;
}

bool Channel::isUserOperator(std::string nickname) const {
	if (this->channel_operators_.find(nickname) != this->channel_operators_.end())
		return true;
	return false;
}

bool Channel::isInvited(const std::string& nickname) const {
	if (this->invited_users_.find(nickname) != this->invited_users_.end())
		return true;
	return false;
}

User* Channel::getUserByNick(std::string nickname) {
	std::map<std::string, User*>::iterator it = channel_members_.find(nickname);
	if (it == this->channel_members_.end())
		return NULL;
	return it->second;
}

/* SETTERS */
void Channel::setChannelTopic(std::string topic) { this->channel_topic_ = topic; }

// Modes:
//       1. 'i': need invitation
//       2. 'k': need password
///      3. 'l': num of user limited
//       4. 'o': give operator privilege
//       5. 't': topic limited

// Operators: '+' (add), '-' (remove)
// Following Values: 
//       1. Password (With k)
//       2. Number of user (With l)
//       3. A nickname (With o)
//       4. A string (with t)
int Channel::setMode(char mode, char op, std::string value, User* setter) {
	if (this->channel_operators_.find(setter->getNickname()) == channel_operators_.end())
		return 482;

	bool is_plus = (op == '+');

	try {
		switch (mode) {
			case 'i': //
				this->is_invite_only_ = is_plus;
				break;
			
			case 't':
				this->is_topic_restricted_ = is_plus;
				break;

			case 'k': // need password
				if (is_plus) {
					if (value.empty()) throw IRCException(461, "ERR_NEEDMOREPARAMS");
					this->channel_password_ = value;
					this->has_key_ = true;
				} else { // => "-k"
					this->channel_password_ = "";
					this->has_key_ = false;
				}
				break;

			case 'l': // Number of user limited
				if (is_plus) {
					if (value.empty()) throw IRCException(461, "ERR_NEEDMOREPARAMS");

					int limit;
					if (!stringToInt(value, limit))
						throw IRCException(472, "ERR_UNKNOWNMORE");
					this->member_limit_ = (limit < 0) ? 0 : limit;
				} else { // => "-l"
					this->member_limit_ = 0;
				}
				break;

			case 'o': {
				if (value.empty()) throw IRCException(461, "ERR_NEEDMOREPARAMS");
				User* target = getUserByNick(value);
				if (!target) throw IRCException(401, "ERR_NOSUCHNICK");

				return is_plus ? this->addOperator(target) : this->removeOperator(target);
			}
			
			default:
				throw IRCException(472, "ERR_UNKNOWNMODE");
		}
	} catch (const IRCException& e) {
		return e.getErrorCode();
	}
	return 0;
}

/* METHODS */

// RFC standards error code used:
// https://www.rfc-editor.org/rfc/rfc2812.html
// 475(ERR_BADCHANNELKEY): Wrong password for the channel
// 471(ERR_CHANNELISFULL): User limit reached
// 0: Success to join a user
int Channel::addUser(User* user, std::string password) {
	// User already exists in the channel
	if (this->channel_members_.find(user->getNickname()) != this->channel_members_.end())
		return 0; // Success

	// Invite-only mode enabled (+i)
	if (this->is_invite_only_) {
		if (this->invited_users_.find(user->getNickname()) == this->invited_users_.end())
			return 473;
	}

	// Password validation (only when +k is enabled)
	if (this->has_key_ && this->channel_password_ != password)
		return 475; // ERR_BADCHANNELKEY

	// User limit check (only when +l is enabled)
	// if member_limit_ is 0, unlimited user entry allowed
	if (this->member_limit_ > 0 && this->channel_members_.size() >= (size_t)this->member_limit_)
		return 471;

	// User entry approval and removes user from the invited user list
	this->channel_members_[user->getNickname()] = user;
	this->invited_users_.erase(user->getNickname());
	user->joinChannel(this);

	return 0; // Success
}

int Channel::removeUser(User* user) {
	std::string nickname = user->getNickname();
	// User doesn't exist in the channel
	if (this->channel_members_.find(nickname) == this->channel_members_.end())
		return 442; // ERR_NOTONCHANNEL

	// Delete user in the channel members
	this->channel_members_.erase(nickname);

	// Delete user in the channel operaters
	this->channel_operators_.erase(nickname);
	user->leaveChannel(this);

	return 0;
}

int Channel::addOperator(User* user) {
	if (!user) return -1;

	std::string nickname = user->getNickname();

	// User is not in the channel members.
	if (this->channel_members_.find(nickname) == this->channel_members_.end())
		return 441; // ERR_USERNOTINCHANNEL

	// User is already an operator
	if (this->channel_operators_.find(nickname) != this->channel_operators_.end())
		return 0;
	
	this->channel_operators_.insert(nickname);
	return 0;
}

int Channel::removeOperator(User* user) {
	if (!user) return -1;
	std::string nickname = user->getNickname();

	// User is not in the channel
	if (this->channel_members_.find(nickname) == this->channel_members_.end())
		return 441; // ERR_USERNOTINCHANNEL

	// User is not an operator
	if (this->channel_operators_.find(nickname) == this->channel_operators_.end())
		return 0;

	// Remove the user from the operator list
	this->channel_operators_.erase(nickname);

	return 0;
}

void Channel::addInvite(const std::string& nickname) {
	this->invited_users_.insert(nickname);

	std::cout << "[Channel] " << channel_name_ << ": " << nickname << " has been invited." << std::endl;
}

void Channel::removeInvite(const std::string& nickname) {
	this->invited_users_.erase(nickname);

	std::cout << "[Channel] " << channel_name_ << ": " << nickname << " removed from invite list." << std::endl;
}

void Channel::updateUserNick(const std::string& old_pick, const std::string& new_nick) {
	std::map<std::string, User*>::iterator it = this->channel_members_.find(old_pick);
	if (it != this->channel_members_.end()) {
		User* user = it->second;
		this->channel_members_.erase(it);
		this->channel_members_[new_nick] = user;
	}

	std::set<std::string>::iterator op_it = this->channel_operators_.find(old_pick);
	if (op_it != this->channel_operators_.end()) {
		this->channel_operators_.erase(op_it);
		this->channel_operators_.insert(new_nick);
	}
}

// Send the message to all members except the sender (except_user)
void Channel::broadcast(std::string msg, User* except_user) {
	if (msg.empty()) return;

	// Add the IRC standard newline character sequence("\r\n"),
	if (msg.find("\r\n") == std::string::npos)
		msg += "\r\n";

	std::map<std::string, User*>::iterator it;
	for (it = this->channel_members_.begin(); it != this->channel_members_.end(); ++it) {
		User* target_user = it->second;

		// If the current target is except user, skip.
		if (except_user != NULL && target_user == except_user)
			continue;

		if (send(target_user->getFd(), msg.c_str(), msg.size(), 0) == -1)
			std::cerr << "Broadcast send failed to: " << target_user->getNickname() << std::endl;
	}
}

// Strict string-to-integer conversion (unlike atoi, it rejects invalid input like "42abc")
bool Channel::stringToInt(const std::string& str, int& result) {
	std::stringstream ss(str);

	if (!(ss >> result) || !ss.eof())
		return false;
	return true;
}
