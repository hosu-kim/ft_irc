#pragma once

#include <string>
#include <map>
#include <set>

class User;

class Channel {
private:
	std::string channel_name_;
	std::string channel_password_;
	std::string channel_topic_;

	std::map<std::string, User*> channel_members_;
	std::set<std::string> channel_operators_;
	std::set<std::string> invited_users_;

	// Mode flags
	bool is_invite_only_; // 'i' mode
	bool is_topic_restricted_; // 't' mode
	bool has_key_; // 'k' mode
	// member_limit_ = 0 means unlimited user entry allowed
	size_t member_limit_; // 'l' mode

	Channel();

public:
	/* Orthodox Canonical Form */
	Channel(const std::string& channel_name, User* channel_operator);
	Channel(const Channel& src);
	Channel& operator=(const Channel& src);
	~Channel();

	/* GETTERS */
	bool hasMode(char mode) const;
	size_t getMemberCount() const;
	std::string getChannelKey() const;
	size_t getMemberLimit() const;
	bool isUserInChannel(std::string userNickname) const;
	bool isUserOperator(std::string userNickname) const;
	std::string getChannelName() const;
	size_t getUserCount() const;
	std::string getChannelTopic() const;
	const std::map<std::string, User*>& getMembers() const;
	User* getUserByNick(std::string nickname);

	/* SETTERS */
	void setChannelTopic(std::string topic);
	int setMode(char mode, char op, std::string value, User* setter);


	/* METHODS */
	int addUser(User* user, std::string password);
	int removeUser(User* user);
	int addOperator(User* user);
	int removeOperator(User* user);
	void addInvite(const std::string& nickname);
	bool isInvited(const std::string& nickname) const;
	void removeInvite(const std::string& nickname);
	void updateUserNick(const std::string& old_nick, const std::string& new_nick);

	/* Except the sender(exceptUser), all members receive the message*/
	void broadcast(std::string message, User* except_user);

	// mode change member functions
	bool stringToInt(const std::string& str, int& result);

	class IRCException : public std::exception {
	private:
		int error_code_;
		std::string message_;

	public:
		IRCException(int code, const std::string& msg)
		: error_code_(code), message_(msg) {}

		virtual ~IRCException() throw() {}

		virtual const char* what() const throw() {
			return message_.c_str();
		}

		int getErrorCode() const {
			return error_code_;
		}
	};
};
