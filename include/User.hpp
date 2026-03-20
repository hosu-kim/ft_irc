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

        std::string getName() const;
        std::string getNickname() const;
        std::string getFullname() const;
        std::string getHostmask() const;
        int getFd() const;

        void setName(std::string _name);
        void setNickname(std::string _nickname);
        void setFullname(std::string _fullname);
        void setHostmask(std::string _hostmask);

	private:

		std::string name;
		std::string nickname;
		std::string fullname;
		std::string hostmask;

        int fd;

		bool	registered;
        bool    has_user;
        bool    has_nick;
        bool    pass_ok;

};