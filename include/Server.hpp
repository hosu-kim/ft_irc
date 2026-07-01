#pragma once

#include <string>
#include <map>
#include <vector>
#include <exception>
#include <netinet/in.h>
#include <poll.h>

#include "User.hpp"
#include "Channel.hpp"

#define MAX_PORT_NUMBER 65535
#define MAX_CLIENTS 1024

class ACmd;

class Server {
	// Stores User objects directly; std::map guarantees pointer stability and automatic cleanup
	typedef std::map<int, User>				userMap;
	// Stores channel pointers to avoid heavy copy overhead of channel objects
	typedef std::map<std::string, Channel*>	channelMap;

	public:
		Server();
		Server(char **argv);
		~Server();

		// GETTERS
		std::string getServerName() const;
		std::string getUserName() const;
		std::string getPassword() const;
		User*		getUserByNick(const std::string& nick);
		Channel*	getChannel(std::string channelName);

		// SETTERS
		void		setSocket();
		Channel*	setChannel(std::string channelName, User* channelOperator);

		// METHODS
		void		run();
		void		newClient(void);
		void		clientRequest(int i);
		void		removeUser(int i);
		void		removeUser(User &user);
		void		removeChannel(std::string channelName);

		class RunTimeError: public std::exception {
			private:
				std::string msg;
			public:
				RunTimeError(const std::string &message) : msg(message) {}
			virtual ~RunTimeError() throw() {}
			virtual const char* what() const throw() {
				return msg.c_str();
			}
		};

	private:
		int 				_port;
		int					_serverFd;
		int					_fdCount;

		std::string			_serverName;
		std::string			_userName;
		std::string 		_password;

		struct sockaddr_in	_serverAddr;
		std::vector<pollfd>	_fds;
		// pollfd => a struct to request poll() to observe several sockets; array to monitor events on server and client sockets
		//           Members: int fd, short events, short revents
		pollfd				_userPoll[MAX_CLIENTS];
		userMap				_users;
		channelMap			_channels;
};