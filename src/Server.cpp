#include "Server.hpp"
#include "CmdFactory.hpp"
#include "CmdNICK.hpp"
#include "CmdJOIN.hpp"
#include <iostream>
#include <cstring>       // memset
#include <unistd.h>      // close
#include <fcntl.h>       // fcntl, 0_NONBLOCK
#include <sys/socket.h> // accept, bind, listen
#include <arpa/inet.h>  // inet_ntoa, htons
#include <csignal>       // signal, SIGINT
#include <cerrno>        // errno, EINTR
#include <cstdlib>       // atoi

static bool g_server_run = true;

// Avoids still reachable memory blocks when to press Ctrl+C on Valgrind
void signal_handler(int signum) {
	(void)signum;
	g_server_run = false;
}

// _serverFd(-1) => -1: signal which shows socket is not opened yet.
Server::Server() : _port(0), _serverFd(-1), _fdCount(0), _serverName("ft_irc"), _userName("localhost") {
	// Initialize _userPoll to 0 to prevent garbage values from causing memory errors
	std::memset(_userPoll, 0, sizeof(_userPoll));
}

Server::Server(char **argv) : _port(atoi(argv[1])), _serverName("ft_irc"), _userName("localhost"), _password(argv[2])
{
	if (_port <= 0 || _port > MAX_PORT_NUMBER)
		throw Server::RunTimeError("Invalid port.");

	// Initialize _userPoll to 0 to prevent garbage values from causing memory errors
	std::memset(_userPoll, 0, sizeof(_userPoll));

	std::cout << "Creating a server..." << std::endl;
}

Server::~Server() {
	std::cout << "Server shutting down..." << std::endl;

	// Free dynamically allocated channels to prevent memory leaks (=heap allocation => Channel* newChan = new...)
	for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		delete it->second;

	for (int i = 0; i < _fdCount; ++i)
		close(_userPoll[i].fd);
}

// GETTERS
std::string Server::getServerName() const { return _serverName; }
std::string Server::getUserName() const { return _userName; }
std::string Server::getPassword() const { return _password; }
User* Server::getUserByNick(const std::string& nick) {
	for (std::map<int, User>::iterator it = _users.begin(); it != _users.end(); ++it) {
		if (it->second.getNickname() == nick)
			return &(it->second);
	}
	return NULL;
}

Channel* Server::getChannel(std::string channelName) {
	// typedef std::map<std::string, Channel*>	channelMap;
	channelMap::iterator it = _channels.find(channelName);
	if (it != _channels.end())
		return it->second;
	return NULL;
}

// SETTERS
void Server::setSocket() {
	// socket(A. domain, B. type, C. protocol)
	// => Create a socket endpoint for network communication
	// A. AF_INET: IPv4 address family
	// B. SOCK_STREAM: Connection-oriented TCP protocol
	// C. 0: Use default protocol for the domain and type
	_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverFd < 0)
		throw Server::RunTimeError("socket() failed.");

	int opt = 1;
	// setsockopt(A. socket, B. level, C. option_name, D. option_val, E. value_len)
	// => Set socket options
	// C. SO_REUSEADDR: Allow immediate port reuse to prevent TIME_WAIT binding errors
	// - TIME_WAIT? When the server exits or crashes, the OS temporarily locks
	//              the port in a TIME_WAIT state (usually for 1-2 minutes) for safety
	if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		throw Server::RunTimeError("Failed to set option (SO_REUSEADDR) on socket.");
	
	// fcntl() => File Control; Set the socket to non-blocking mode
	// - F_SETFL: Set fiile status flags
	// - O_NONBLOCK: Enable non-blocking I/O
	// - Non-blocking? Makes socket calls (like accept/recv) return immediately without waiting
	if (fcntl(_serverFd, F_SETFL, O_NONBLOCK) == -1)
		throw Server::RunTimeError("fcntl() failed.");
	
	memset(&_serverAddr, 0, sizeof(_serverAddr));
	_serverAddr.sin_family = AF_INET; // IP4v
	_serverAddr.sin_addr.s_addr = INADDR_ANY; // Allows every connection from any IP address
	_serverAddr.sin_port = htons(_port); // Convert port number to network byte order

	// Bind the socket to the server address
	if (bind(_serverFd, (struct sockaddr *)&_serverAddr, sizeof(_serverAddr)) == -1)
		throw Server::RunTimeError("Failed to bind socket.");
	
	// Start listening for incoming connections with the maximum backlog queue
	if (listen(_serverFd, SOMAXCONN) < 0)
		throw Server::RunTimeError("listen() failed.");

	_userPoll[0].fd = _serverFd;
	_userPoll[0].events = POLLIN; // Monitor for incoming connections
	_userPoll[0].revents = 0;
	_fdCount = 1;
	std::cout << "setSocket() finished..." << std::endl;
}

Channel* Server::setChannel(std::string channelName, User* channelOperator) {
	Channel* newChannel = new Channel(channelName, channelOperator);
	_channels[channelName] = newChannel;

	std::cout << "[Server] New channel created: " << channelName << " by " << channelOperator->getNickname() << std::endl;
	return newChannel;
}

void Server::newClient(void)
{
	int user_fd;
	struct sockaddr_in useraddr;
	socklen_t addrlen = sizeof(useraddr);

	user_fd = accept(_serverFd, (sockaddr *)&useraddr, &addrlen);

	if (user_fd < 0) {
		if (errno == EWOULDBLOCK || errno == EAGAIN)
			return;
		else
			throw Server::RunTimeError("accept() failed.");
	}
	if (_fdCount >= MAX_CLIENTS) {
		close(user_fd);
		return;
	} else {
		fcntl(user_fd, F_SETFL, O_NONBLOCK);
		this->_userPoll[_fdCount].fd = user_fd;
		this->_userPoll[_fdCount].events = POLLIN;
		this->_userPoll[_fdCount].revents = 0;
		this->_fdCount++;
		this->_users[user_fd] = User(user_fd);
		this->_users[user_fd].setHostmask(inet_ntoa(useraddr.sin_addr));
	}
}

void    Server::clientRequest(int i)
{
	char buffer[1024];
	int bytes = recv(_userPoll[i].fd, buffer, sizeof(buffer), 0);
	int fd = _userPoll[i].fd;

	if (bytes > 0)
	{
		std::string message(buffer, bytes);

		_users[fd].buffer += message;

		std::string &buf = _users[fd].buffer;

		while (true)
		{
			size_t pos = buf.find("\n");

			if (pos == std::string::npos)
				break;
			std::string line = buf.substr(0, pos);

			if (!line.empty() && line[line.size() - 1] == '\r')
				line.erase(line.size() - 1);

			buf.erase(0, pos + 1);

			if (line.empty())
				continue;

			std::vector<std::string> tokens = CmdFactory::splitBySpaces(line);
			if (tokens.empty())
				continue;

			std::string cmdName = tokens[0];

			// Capizalizes user input: e.g., join -> JOIN
			for (size_t i = 0; i <cmdName.size(); ++i) {
				cmdName[i] = std::toupper(cmdName[i]);
			}

			std::cout << "cmd = " << cmdName << std::endl;

			User &user = _users[fd];
			if (!user.getRegistered() &&
				cmdName != "PASS" && cmdName != "NICK" &&
				cmdName != "USER" && cmdName != "QUIT" &&
				cmdName != "PING" && cmdName != "PONG") {
				std::string nick = user.getNickname().empty() ? "*" : user.getNickname();
				std::string errMsg = ":" + this->getServerName() + " 451 " + nick + " :You have not registered";
				_users[fd].reply(errMsg);
				continue;
			}

			ACmd* command = CmdFactory::createCommand(tokens);
	
			if (command != NULL) {
				try {
					command->execute(user, *this);
				} catch (const std::exception& e) {
					std::cerr << "Error: Exception occurred during command execution: " << e.what() << std::endl;
				}
				delete command;
			} else {
				std::string nick = user.getNickname().empty() ? "*" : user.getNickname();
				std::string errMsg = ":" + this->getServerName() + " 421 " + nick + " " + cmdName + " :Unknown command";
				user.reply(errMsg);
			}

			if (this->_users.find(fd) == this->_users.end()) {
				return;
			}
		}
	}
	else if (bytes == 0)
	{
		std::cout << "Client disconnected: " << fd << std::endl;
		removeUser(i);
	}
	else
	{
		if (errno == EWOULDBLOCK || errno == EAGAIN)
			return;
		else
			removeUser(i);
	}
}

void Server::removeUser(int i)
{
	int fd = _userPoll[i].fd;
	userMap::iterator it = _users.find(fd);

	if (it != _users.end()) {
		User &user = it->second;
		std::map<std::string, Channel*> joinedChannels = user.getJoinedChannels();
		for (std::map<std::string, Channel*>::iterator chanIt = joinedChannels.begin(); chanIt != joinedChannels.end(); ++chanIt) {
			Channel* channel = chanIt->second;

			channel->removeUser(&user);
			if (channel->getMemberCount() == 0) {
				this->removeChannel(channel->getChannelName());
			}
		}
	}

	close(fd);
	_users.erase(fd);
	if (i < (int)_fdCount - 1) {
		_userPoll[i] = _userPoll[_fdCount - 1];
	}
	_fdCount--;
	
	std::cout << "[Server] User removed. Current count: " << _fdCount << std::endl;
}

void Server::removeUser(User &user) {
	int fd = user.getFd();

	for (int i = 0; i < _fdCount; i++) {
		if (_userPoll[i].fd == fd) {
			this->removeUser(i);
			return;
		}
	}
}

void	Server::removeChannel(std::string channelName) {
	channelMap::iterator it = _channels.find(channelName);
	if (it != _channels.end()) {
		delete it->second;
		_channels.erase(it);
	}
}

void Server::run()
{
	signal(SIGINT, signal_handler);

	setSocket();

	while (g_server_run)
	{
		if (poll(_userPoll, _fdCount, -1) < 0) {
			if (errno == EINTR) // Avoids poll failure by Ctrl+C etc.
				continue;
			throw Server::RunTimeError("Error: poll failed.");
		}
		for (int i = 0; i < _fdCount; i++) {
			if (_userPoll[i].revents & POLLIN) {
				if (_userPoll[i].fd == _serverFd) {
					newClient();
				}
				else {
					int count_before = _fdCount;
					clientRequest(i);

					if (_fdCount < count_before) {
						i--;
						continue;
					}
				}
			}
			else if (_userPoll[i].revents & (POLLHUP | POLLERR)) {
				removeUser(i);
				i--;
				continue;
			}
			_userPoll[i].revents = 0;
		}
	}
}
