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
//#include "ACmd.hpp"
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

class ACmd;

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
		std::string getPassword() const;
		Channel*	getChannel(std::string channelName);
		User*		getUserByNick(const std::string& nick);

		// SETTERS
		void		setSocket();
		Channel*	setChannel(std::string channelName, User* channelOperator);

		// MEMBER FUNCTIONS
		void 	run();
		void	newClient(void);
		void	clientRequest(int i);
		void	removeUser(int i);
		void	removeUser(User &user);
		void	removeChannel(std::string channelName);

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
		int 				_port;
		int					_server_fd;
		int					_fd_count;

		std::string			_serverName;
		std::string			_userName;
		std::string 		_password;

		struct sockaddr_in	_addr;
		std::vector<pollfd>	_fds;
		pollfd				_user_poll[MAX_CLIENTS];
		user_map			_users;
		channel_map			_channels;
};