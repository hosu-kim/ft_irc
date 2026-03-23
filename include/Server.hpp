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
#include "ACmd.hpp"
#include "Channel.hpp"
#include "CmdFactory.hpp"

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
	typedef std::map<int, User>				user_map;
	typedef std::map<std::string, Channel*>	channel_map;

	public:
		Server();
		Server(char **argv);
		~Server();

		// GETTERS
		std::string getUserName() const;
		std::string getServerName() const;

		// MEMBER FUNCTIONS
		void 	run();
		void 	setSocket();
		void	newClient(void);
		void	clientRequest(int i);
		void	removeUser(int i);

		User*		findUserByNick(const std::string& nick);
		Channel*	findChannel(std::string channelName);
		Channel*	createChannel(std::string channelName, User* channelOperator);

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
		std::string			_serverName;
		std::string			_userName;
		int 				_port;
		std::string 		_password;
		int					_server_fd;
		struct sockaddr_in	_addr;
		std::vector<pollfd>	_fds;
		pollfd				_user_poll[MAX_CLIENTS];
		int					_fd_count;
		user_map			_users;
		channel_map			_channels;
};