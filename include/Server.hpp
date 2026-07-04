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
private:
	// Stores User objects directly; std::map guarantees pointer stability and automatic cleanup
	typedef std::map<int, User>				UserMap;
	// Stores channel pointers to avoid heavy copy overhead of channel objects
	typedef std::map<std::string, Channel*>	ChannelMap;

	int 				port_;
	int					server_fd_;
	int					fd_count_;

	std::string			server_name_;
	std::string			user_name_;
	std::string 		password_;

	struct sockaddr_in	server_addr_;
	std::vector<pollfd>	fds_;
	// pollfd => a struct to request poll() to observe several sockets; array to monitor events on server and client sockets
	//           Members: int fd, short events, short revents
	pollfd				user_poll_[MAX_CLIENTS];
	UserMap				users_;
	ChannelMap			channels_;

public:
	Server();
	Server(char **argv);
	~Server();

	/* GETTERS */
	std::string getServerName() const;
	std::string getUserName() const;
	std::string getPassword() const;
	User*		getUserByNick(const std::string& nick);
	Channel*	getChannel(std::string channel_name);

	/* SETTERS */
	void		setSocket();
	Channel*	setChannel(std::string channel_name, User* channel_operator);

	/* METHODS */
	void		run();
	void		newClient(void);
	void		clientRequest(int i);
	void		removeUser(int i);
	void		removeUser(User &user);
	void		removeChannel(std::string channel_name);

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
};