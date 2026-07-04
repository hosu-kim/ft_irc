#pragma once

#include <string>
#include <map>
#include <ctime>

class Channel;

class User {
private:

	int fd_;

	std::string user_name_;
	std::string nickname_;
	std::string real_name_;
	std::string host_name_;

	std::map<std::string, Channel*> joined_channels_;

	bool is_registered_;
	bool has_user_;
	bool has_nick_;
	bool pass_ok_;
	
	time_t last_activity_;

public:
	User();
	User(int user_fd);

	std::string buffer_;

	// GETTERS
	std::string getUserName() const;
	std::string getNickname() const;
	std::string getFullname() const;
	std::string getHostName() const;
	const std::map<std::string, Channel*>& getJoinedChannels() const;

	int getFd() const;

	bool isRegistered() const;
	bool getHasUser() const;
	bool getHasNick() const;
	bool getPassOK() const;


	// SETTERS
	void setUserName(std::string user_name);
	void setNickname(std::string nickname);
	void setRealName(std::string real_name);
	void setHostmask(std::string hostmask);

	void setRegistered(bool flag);
	void setHasUser(bool flag);
	void setHasNick(bool flag);
	void setPassOK(bool flag);

	// ACTIVITY TRACKING
	void updateActivity();
	time_t getLastActivity() const;

	// MEMBER FUNCTIONS
	void reply(std::string msg);
	void joinChannel(Channel* channel);
	void leaveChannel(Channel* channel);
};