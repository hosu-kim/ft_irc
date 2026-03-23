#pragma once

#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>

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

		int 	getFd() const;

		bool    getRegistered();
		bool    getHasUser();
		bool    getHasNick();
		bool    getPassOK();


		// SETTERS
		void setName(std::string _userName);
		void setNickname(std::string _nickname);
		void setFullname(std::string _fullname);
		void setHostmask(std::string _hostmask);

		void    setRegistered(bool flag);
		void    setHasUser(bool flag);
		void    setHasNick(bool flag);
		void    setPassOK(bool flag);

		// MEMBER FUNCTIONS
		void reply(std::string msg);

	private:

		std::string userName;
		std::string nickname;
		std::string fullname;
		std::string hostName;

		int fd;

		bool	registered;
		bool    has_user;
		bool    has_nick;
		bool    pass_ok;

};