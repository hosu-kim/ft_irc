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

#include "User.hpp"

#include <map>
#include <exception>
#include <string>
#include <sstream>

#include <stdlib.h>

#define MAX_PORT_NUMBER 65535

class Server
{
	typedef std::map<int, User>	usermap;

	public:
		Server(char **argv);
		~Server();

		void run();
		void setSocket();

		class WrongArgumentCount : public std::exception
		{
			private:
				std::string msg;
			public:
				WrongArgumentCount(int val)
				{
					std::ostringstream oss;
					oss << "Argc should be 3 but is " << val;
					msg = oss.str();
				}
				virtual ~WrongArgumentCount() throw() {}
				virtual const char* what() const throw()
				{
					return msg.c_str();
				}
		};

	private:
		int port;
		std::string password;
};