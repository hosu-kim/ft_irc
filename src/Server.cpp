#include "Server.hpp"
#include "CmdFactory.hpp"
#include "commands/CmdNICK.hpp"
#include "commands/CmdJOIN.hpp"

Server::Server() : _port(0), _server_fd(-1), _fd_count(0), _serverName("ft_irc"), _userName("localhost") {}

Server::Server(char **argv) : _port(atoi(argv[1])), _serverName("ft_irc"), _userName("localhost"), _password(argv[2])
{
	if (_port <= 0 || _port > MAX_PORT_NUMBER)
		throw Server::RunTimeError("Invalid port.");
	
	std::cout << "Creating a server..." << std::endl;
}

Server::~Server()
{
	std::cout << "Server shutting down..." << std::endl;
	for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
		delete it->second;

	}

	for (int i = 0; i < _fd_count; ++i) {
		close(_user_poll[i].fd);
	}
}

std::string Server::getUserName() const { return _userName; }
std::string Server::getServerName() const { return _serverName; }
std::string Server::getPassword() const { return _password; }

void Server::setSocket()
{
	_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_fd < 0)
		throw Server::RunTimeError("socket() failed.");

	int opt = 1;
	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		throw Server::RunTimeError("Failed to set option (SO_REUSEADDR) on socket.");
	
	if (fcntl(_server_fd, F_SETFL, O_NONBLOCK) == -1)
		throw Server::RunTimeError("fcntl() failed.");
	
	memset(&_addr, 0, sizeof(_addr));
	_addr.sin_family = AF_INET;
	_addr.sin_addr.s_addr = INADDR_ANY;
	_addr.sin_port = htons(_port);

	if (bind(_server_fd, (struct sockaddr *)&_addr, sizeof(_addr)) == -1)
		throw Server::RunTimeError("Failed to bind socket.");
	
	if (listen(_server_fd, SOMAXCONN) < 0)
		throw Server::RunTimeError("listen() failed.");

	_user_poll[0].fd = _server_fd;
	_user_poll[0].events = POLLIN;
	_fd_count = 1;
	std::cout << "setSocket() finished..." << std::endl;
}


void    Server::newClient(void)
{
	int user_fd;
	struct sockaddr_in   useraddr;
	socklen_t   addrlen = sizeof(useraddr);


	while (true)
	{
		user_fd = accept(_server_fd, (sockaddr *)&useraddr, &addrlen);

		if (user_fd < 0)
		{
			if (errno == EWOULDBLOCK || errno == EAGAIN)
				break;
			else
				throw Server::RunTimeError("accept() failed.");
		}
		if (_fd_count >= MAX_CLIENTS)
		{
			close(user_fd);
			return;
		}
		else
		{
			fcntl(user_fd, F_SETFL, O_NONBLOCK);
			this->_user_poll[_fd_count].fd = user_fd;
			this->_user_poll[_fd_count].events = POLLIN;
			this->_fd_count++;
			this->_users[user_fd] = User(user_fd);
			this->_users[user_fd].setHostmask(inet_ntoa(useraddr.sin_addr));
		}
	}
}

User* Server::getUserByNick(const std::string& nick)
{
	for (std::map<int, User>::iterator it = _users.begin(); it != _users.end(); ++it)
	{
		if (it->second.getNickname() == nick)
			return &(it->second);
	}
	return NULL;
}

Channel* Server::getChannel(std::string channelName) {
	channel_map::iterator it = _channels.find(channelName);
	if (it != _channels.end())
		return it->second; // found
	return NULL;
}

Channel* Server::setChannel(std::string channelName, User* channelOperator) {
	Channel* newChannel = new Channel(channelName, channelOperator);
	_channels[channelName] = newChannel;

	std::cout << "[Server] New channel created: " << channelName << " by " << channelOperator->getNickname() << std::endl;
	return newChannel;
}

void    Server::clientRequest(int i)
{
	char buffer[1024];
	int bytes = recv(_user_poll[i].fd, buffer, sizeof(buffer), 0);
	int fd = _user_poll[i].fd;

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
	int fd = _user_poll[i].fd;
	user_map::iterator it = _users.find(fd);

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
	if (i < (int)_fd_count - 1) {
		_user_poll[i] = _user_poll[_fd_count - 1];
	}
	_fd_count--;
	
	std::cout << "[Server] User removed. Current count: " << _fd_count << std::endl;
}

void Server::removeUser(User &user) {
	int fd = user.getFd();

	for (int i = 0; i < _fd_count; i++) {
		if (_user_poll[i].fd == fd) {
			this->removeUser(i);
			return;
		}
	}
}

void	Server::removeChannel(std::string channelName) {
	channel_map::iterator it = _channels.find(channelName);
	if (it != _channels.end()) {
		delete it->second;
		_channels.erase(it);
	}
}

void Server::run()
{
	setSocket();

	while (true)
	{
		if (poll(_user_poll, _fd_count, -1) < 0)
			throw Server::RunTimeError("Error: poll failed.");
		for (int i = 0; i < _fd_count; i++) {
			if (_user_poll[i].revents & POLLIN) {
				if (_user_poll[i].fd == _server_fd) {
					newClient();
				}
				else {
					int count_before = _fd_count;
					clientRequest(i);

					if (_fd_count < count_before) {
						i--;
						continue;
					}
				}
			}
			else if (_user_poll[i].revents & (POLLHUP | POLLERR)) {
				removeUser(i);
				i--;
				continue;
			}
			_user_poll[i].revents = 0;
		}
	}
}
