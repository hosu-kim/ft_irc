#pragma once

#include <string>
#include <map>
#include <ctime>

class Channel;

class User
{
	public:
		User();
		User(int user_fd);

		std::string buffer;

		// GETTERS
		std::string getUserName() const;
		std::string getNickname() const;
		std::string getFullname() const;
		std::string getHostName() const;
		const std::map<std::string, Channel*>& getJoinedChannels() const;

		int 	getFd() const;

		bool    getRegistered();
		bool    getHasUser();
		bool    getHasNick();
		bool    getPassOK();


		// SETTERS
		void setUserName(std::string _userName);
		void setNickname(std::string _nickname);
		void setRealName(std::string _realName);
		void setHostmask(std::string _hostmask);

		void    setRegistered(bool flag);
		void    setHasUser(bool flag);
		void    setHasNick(bool flag);
		void    setPassOK(bool flag);

		// ACTIVITY TRACKING
		void    updateActivity();
		time_t  getLastActivity() const;

		// MEMBER FUNCTIONS
		void reply(std::string msg);
		void joinChannel(Channel* channel);
		void leaveChannel(Channel* channel);

	private:

		int _fd;

		std::string _userName;
		std::string _nickname;
		std::string _realName;
		std::string _hostName;

		std::map<std::string, Channel*> _joinedChannels;

		bool	registered;
		bool    _hasUser;
		bool    _hasNick;
		bool    _passOk;
		
		time_t  _lastActivity;

};