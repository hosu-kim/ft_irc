#include "Server.hpp"

// ========================== ORTHODOX CANONICAL FORM ==========================
Server::Server() : _userName("localhost"), _port(0), _server_fd(-1), _fd_count(0) {}

Server::Server(char **argv) : _userName("localhost"), _port(atoi(argv[1])), _password(argv[2])
{
	if (_port <= 0 || _port > MAX_PORT_NUMBER)
		throw Server::RunTimeError("Invalid port.");
	
	std::cout << "Creating a server..." << std::endl;
}

Server::~Server()
{
	std::cout << "Server shutting down..." << std::endl;
	close(_server_fd);
}
//==============================================================================

/* ***GETTERS*** */
std::string Server::getUserName() const { return _userName; }
std::string Server::getServerName() const { return _serverName; }

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
	_user_poll[0].events = POLLIN /*| POLLHUP | POLLOUT*/;
	_fd_count = 1;
	std::cout << "setSocket() finished..." << std::endl;
}


void    Server::newClient(void)
{
	// GOAL: accept all pending clients and add them to poll()
	// On server_fd: one POLLIN != one client!!!!
	// There may be multiple queued 
	// while (true)
	// {
	//      int client_fd = accept(...); ------ + check for fail
	//      if (client_fd < 0)
	//          break when no more clients
	//      else -> handle client
	//          1. Set non-blocking: fcntl(client_fd, F_SETFL, O_NONBLOCK;)
	//          2. Add to poll array:
	//              user_poll[fd_count].fd = clinet_fd;
	//              user_poll[fd_count].events = POLLIN;
	//              fd_count++;
	//          3. Add to user map:
	//              users[client_fd] = User(client_fd);
	//          4. Check for fd_count >= MAX_CLIENTS -> close client, refuse connection
	// }

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
			_user_poll[_fd_count].fd = user_fd;
			_user_poll[_fd_count].events = POLLIN;
			_fd_count++;
			_users[user_fd] = User(user_fd);
		}
	}
}

User* Server::findUserByNick(const std::string& nick)
{
	for (std::map<int, User>::iterator it = _users.begin(); it != _users.end(); ++it)
	{
		if (it->second.getNickname() == nick)
			return &(it->second);
	}
	return NULL;
}

/* finds the given channel userName in the _channels map container
   and return the corresponding iterator if not found return `NULL`
 */
Channel* Server::findChannel(std::string channelName) {
	channel_map::iterator it = _channels.find(channelName);
	if (it != _channels.end())
		return it->second; // found
	return NULL;
}

/* Creates a channel instance and add it into the _channels map container
   and returns it
*/
Channel* Server::createChannel(std::string channelName, User* channelOperator) {
	Channel* newChannel = new Channel(channelName, channelOperator);
	_channels[channelName] = newChannel;

	std::cout << "[Server] New channel created: " << channelName << " by " << channelOperator->getNickname() << std::endl;
	return newChannel;
}

void    Server::clientRequest(int i)
{
	// GOAL: read data from a clinet safely
	char buffer[1024];
	int bytes = recv(/*server_fd*/ _user_poll[i].fd, buffer, sizeof(buffer), 0);
	int fd = _user_poll[i].fd;

	if (bytes > 0)
	// Client sent data -> append to user buffer, parse commands
	{
		std::string message(buffer, bytes);

		_users[fd].buffer += message;

		// while (buffer contains "\r\n")   ------ size_t pos = buffer.find("\r\n");
		// {
		//      extract one line ---------- substr(0, pos);
		//      remove it from buffer   --------- erase(0, pos + 2);
		//      process it
		// }

		std::string &buf = _users[fd].buffer;

		while (true)
		{
			//size_t pos = buf.find("\r\n");
			size_t pos = buf.find("\n");

			if (pos == std::string::npos)
				break;
			std::string line = buf.substr(0, pos);

			if (!line.empty() && line[line.size() - 1] == '\r')
				line.erase(line.size() - 1);

			//buf.erase(0, pos + 2);
			buf.erase(0, pos + 1);

			if (line.empty())
				continue;

			std::vector<std::string> tokens = CmdFactory::splitBySpaces(line);
			if (tokens.empty())
				continue;

			std::string cmd = tokens[0];
			std::vector<std::string> params;
			for (size_t i = 1; i < tokens.size(); i++)
				params.push_back(tokens[i]);

			std::cout << "cmd = " << tokens[0] << std::endl;
			if (cmd == "PASS")
			{
				if (tokens.size() < 2)
					continue;
				if (tokens[1] == _password)
				{
					_users[fd].setPassOK(true);

					//DEBUG
					std::string temp1 = "Password set successfully. \r\n";
					std::cout << temp1 << std::endl << "PasswordOK is: " << _users[fd].getPassOK() << std::endl;
				}
			}
			else if (cmd == "USER")
			{
				if (tokens.size() > 4)
				{
					_users[fd].setFullname(tokens[4]);
					_users[fd].setHasUser(true);

					//DEBUG
					std::string temp1 = "User set successfully. \r\n";
					std::cout << temp1 << std::endl << "UserOK is: " << _users[fd].getHasUser() << std::endl;
					std::cout << "User is: " << _users[fd].getFullname() << std::endl;
				}
			}
			else if (cmd == "NICK")
			{
				/*if (tokens.size() > 1)
				{
					_users[fd].setNickname(tokens[1]);
					_users[fd].setHasNick(true);

					//DEBUG
					std::string temp1 = "Nickname set successfully. \r\n";
					std::cout << temp1 << std::endl << "NicknameOK is: " << _users[fd].getHasNick() << std::endl;
					std::cout << "Nickname is: " << _users[fd].getNickname() << std::endl;
				}*/
				CmdNick nickCmd(cmd, params);
				nickCmd.execute(_users[fd], *this);
				
			}
			else if (cmd == "PRIVMSG")
			{
				// 1. Validate input:
				if (tokens.size() < 3)
				{
				//      send error: ERR_NEEDMOREPARAMS
					  continue;
				}

				if (!_users[fd].getRegistered())
					  continue;

				// 2. Extract data:

				std::string target = tokens[1];
				std::string text = tokens[2];

				if (target[0] == '#')
				{
				//    Channel ch; // set to the right channel
				//      ->channel message (LATER)
				//    std::string msg = ":" + text;
					//ch.broadcast(msg, users[fd]);
				// TODO later
					continue;
				}
				
				// else -> user message

				else
				{
					User *targetUser = findUserByNick(target);
					if (!targetUser)
					{
						std::string err = ":localhost 401 " + _users[fd].getNickname() + " " + target +" :No such nick\r\n";
						send(fd, err.c_str(), err.size(), 0);
						continue;
					}
					int target_fd = (*targetUser).getFd();
					std::string msg = ":" + _users[fd].getNickname() + " PRIVMSG " + target + " :" + text + "\r\n";
					send(target_fd, msg.c_str(), msg.size(), 0);
				}
				// 3. Find user (user message):
				// User *targetUser = findUserByNick(target); ------- implement this (loop through users map)
				//         then send message: 
				//              std::string msg = ":" + users[fd].getNickName() + " PRIVMSG " + target + " :" + text + "\r\n";
				//              send(target_fd, msg.c_str(), msg.size(), 0);

				// 4. Channel case - later (use Channel::broadcast())
			}
			else if (cmd == "JOIN")
			{
				/*if (tokens.size() < 2)
					continue;

				std::string channelName = tokens[1];
				Channel* ch = findChannel(channelName);
				if (!ch)
					ch = createChannel(channelName, &_users[fd]);
				int result = ch->addUser(&_users[fd], "");
				std::string joinMsg = ":" + _users[fd].getNickname() + " JOIN " + channelName + "\r\n";
				ch->broadcast(joinMsg, NULL);*/
				CmdJoin joinCmd(cmd, params);
				joinCmd.execute(_users[fd], *this);
			}

			if (!_users[fd].getRegistered() && _users[fd].getPassOK() && _users[fd].getHasNick() && _users[fd].getHasUser())
			{
				_users[fd].setRegistered(true);
				std::string welcome = ":" + std::string("localhost") + " 001 " + _users[fd].getNickname() + " :Welcome to our IRC server\r\n";
				send(fd, welcome.c_str(), welcome.size(), 0);
			}
		}
	}
	else if (bytes == 0)
	{
		// disconnect client
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

void    Server::removeUser(int i)
{
	// GOAL: fully remove a client
	// 1. Get fd:
	int fd = _user_poll[i].fd;
	// 2. Close socket:
	close(fd);
	// 3. Remove from user map:
	_users.erase(fd);
	// 4. Remove from poll array - shift everything left:
	for (int j = i; j < _fd_count -1; j++)
		_user_poll[j] = _user_poll[j + 1];
	_fd_count--;
}


void Server::run()
{
	setSocket(); //add check for success..?

	while (true)
	{
//      std::cout << "entering poll loop..." << std::endl;
		if (poll(_user_poll, _fd_count, -1) < 0)          // change -1 to 0
			throw Server::RunTimeError("Error: poll failed.");
		//unsigned int current_size = fd_count;
		for (unsigned int i = 0; i < _fd_count; i++)
		{
			try
			{
				if (_user_poll[i].revents & POLLIN)
				{
					if (_user_poll[i].fd == _server_fd)
						newClient(); 
					else
						clientRequest(i); 
				}
				else if ((_user_poll[i].revents & (POLLHUP | POLLERR)) /*|| (user_poll[i].revents & POLLOUT)*/)
				{
					removeUser(i);
					i--;
				}
			}
			catch (std::exception &e)
			{
				std::cerr << e.what() << '\n';
			}
			_user_poll[i].revents = 0;
		}
	}
}
