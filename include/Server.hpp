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
#include "commands/ACommand.hpp"
#include "Channel.hpp"

#include <map>
#include <exception>
#include <string>
#include <sstream>

#include <stdlib.h>

#include <cerrno>
#include <vector>

#define MAX_PORT_NUMBER 65535
#define MAX_CLIENTS 1024

class Server
{
	typedef std::map<int, User>	usermap;

	public:
		Server(char **argv);
		~Server();

		void 	run();
		void 	setSocket();
		void	newClient(void);
		void	clientRequest(int i);
		void	removeUser(int i);

		User*	findUserByNick(const std::string& nick);

		class RunTimeError: public std::exception
		{
			private:
				std::string msg;
			public:
				RunTimeError(const std::string &message) : msg(message)
				{}
			virtual ~RunTimeError() throw() 
			{}
			virtual const char* what() const throw()
			{
				return msg.c_str();
			}
		};

	private:
		int 				port;
		std::string 		password;
		int					server_fd;
		struct	sockaddr_in addr;
		std::vector<pollfd> fds;
		pollfd				user_poll[MAX_CLIENTS];
		int					fd_count;
		usermap				users;
};