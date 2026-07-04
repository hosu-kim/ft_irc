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

volatile sig_atomic_t g_server_run = 1;

// Prevent still reachable memory leaks when terminating the server with Ctrl+C under Valgrind
void signalHandler(int signum) {
	(void)signum;
	g_server_run = 0;
}

// _serverFd(-1) => -1: signal which shows socket is not opened yet.
Server::Server() : port_(0), server_fd_(-1), fd_count_(0), server_name_("ft_irc"), user_name_("localhost") {
	// Initialize user_poll_ to 0 to prevent garbage values from causing memory errors
	std::memset(user_poll_, 0, sizeof(user_poll_));
}

Server::Server(char **argv) : port_(atoi(argv[1])), server_name_("ft_irc"), user_name_("localhost"), password_(argv[2])
{
	if (port_ <= 0 || port_ > MAX_PORT_NUMBER)
		throw Server::RunTimeError("Invalid port.");

	// Initialize user_poll_ to 0 to prevent garbage values from causing memory errors
	std::memset(user_poll_, 0, sizeof(user_poll_));

	std::cout << "Creating a server..." << std::endl;
}

Server::~Server() {
	std::cout << "Server shutting down..." << std::endl;

	// Free dynamically allocated channels to prevent memory leaks (=heap allocation => Channel* newChan = new...)
	for (std::map<std::string, Channel*>::iterator it = channels_.begin(); it != channels_.end(); ++it)
		delete it->second;

	for (int i = 0; i < fd_count_; ++i)
		close(user_poll_[i].fd);
}

/* GETTERS */
std::string Server::getServerName() const { return this->server_name_; }

std::string Server::getUserName() const { return this->user_name_; }

std::string Server::getPassword() const { return this->password_; }

User* Server::getUserByNick(const std::string& nick) {
	for (std::map<int, User>::iterator it = users_.begin(); it != users_.end(); ++it) {
		if (it->second.getNickname() == nick)
			return &(it->second);
	}
	return NULL;
}

Channel* Server::getChannel(std::string channel_name) {
	// typedef std::map<std::string, Channel*>	ChannelMap;
	ChannelMap::iterator it = channels_.find(channel_name);
	if (it != channels_.end())
		return it->second;
	return NULL;
}

/* SETTERS */
void Server::setSocket() {
	// socket(A. domain, B. type, C. protocol)
	// => Create a socket endpoint for network communication
	// A. AF_INET: IPv4 address family
	// B. SOCK_STREAM: Connection-oriented TCP protocol
	// C. 0: Use default protocol for the domain and type
	server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd_ < 0)
		throw Server::RunTimeError("socket() failed.");

	int opt = 1;
	// setsockopt(A. socket, B. level, C. option_name, D. option_val, E. value_len)
	// => Set socket options
	// C. SO_REUSEADDR: Allow immediate port reuse to prevent TIME_WAIT binding errors
	// - TIME_WAIT? When the server exits or crashes, the OS temporarily locks
	//              the port in a TIME_WAIT state (usually for 1-2 minutes) for safety
	if (setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		throw Server::RunTimeError("Failed to set option (SO_REUSEADDR) on socket.");
	
	// fcntl() => File Control; Set the socket to non-blocking mode
	// - F_SETFL: Set fiile status flags
	// - O_NONBLOCK: Enable non-blocking I/O
	// - Non-blocking? Makes socket calls (like accept/recv) return immediately without waiting
	if (fcntl(server_fd_, F_SETFL, O_NONBLOCK) == -1)
		throw Server::RunTimeError("fcntl() failed.");

	memset(&server_addr_, 0, sizeof(server_addr_));
	// sin_: Sockaddr INternet
	server_addr_.sin_family = AF_INET; // IP4v
	server_addr_.sin_addr.s_addr = INADDR_ANY; // Allows every connection from any IP address
	server_addr_.sin_port = htons(port_); // Convert port number to network byte order

	// Bind the socket to the server address
	if (bind(server_fd_, (struct sockaddr *)&server_addr_, sizeof(server_addr_)) == -1)
		throw Server::RunTimeError("Failed to bind socket.");

	// Start listening for incoming connections with the maximum backlog queue
	if (listen(server_fd_, SOMAXCONN) < 0)
		throw Server::RunTimeError("listen() failed.");

	user_poll_[0].fd = server_fd_;
	user_poll_[0].events = POLLIN; // Monitor for incoming connections
	user_poll_[0].revents = 0;
	fd_count_ = 1;
	std::cout << "setSocket() finished..." << std::endl;
}

Channel* Server::setChannel(std::string channel_name, User* channel_operator) {
	Channel* newChannel = new Channel(channel_name, channel_operator);
	channels_[channel_name] = newChannel;

	std::cout << "[Server] New channel created: " << channel_name << " by " << channel_operator->getNickname() << std::endl;
	return newChannel;
}

/* METHODS */
void Server::newClient(void)
{
	int user_fd;
	struct sockaddr_in user_addr;
	socklen_t addr_len = sizeof(user_addr);

	// accept(A. listening_socket, B. connect, C. len_of_connect)
	user_fd = accept(server_fd_, (sockaddr *)&user_addr, &addr_len);

	if (user_fd < 0) {
		if (errno == EWOULDBLOCK || errno == EAGAIN)
			return;
		else
			throw Server::RunTimeError("accept() failed.");
	}

	if (fd_count_ >= MAX_CLIENTS) {
		close(user_fd);
		return;
	} else {
		fcntl(user_fd, F_SETFL, O_NONBLOCK);
		this->user_poll_[fd_count_].fd = user_fd;
		this->user_poll_[fd_count_].events = POLLIN;
		this->user_poll_[fd_count_].revents = 0;
		this->fd_count_++;
		this->users_[user_fd] = User(user_fd);
		this->users_[user_fd].setHostmask(inet_ntoa(user_addr.sin_addr));
	}
}

// Read incoming data from the client and append it to the session buffer
void Server::clientRequest(int i) {
	char buffer[1024];
	// recv(): "receive"; reads data sent by the client form a connected socket.
	int bytes_read = recv(user_poll_[i].fd, buffer, sizeof(buffer), 0);
	int user_fd = user_poll_[i].fd;

	if (bytes_read > 0) {
		std::string message(buffer, bytes_read);

		users_[user_fd].buffer_ += message;

		std::string &buf_ref = users_[user_fd].buffer_;

		// Extract and process complete messages delimied by newline (\n), stripping carriage returns (\n)
		while (true) {
			size_t pos = buf_ref.find("\n");

			if (pos == std::string::npos)
				break;
			std::string line = buf_ref.substr(0, pos);

			if (!line.empty() && line[line.size() - 1] == '\r')
				line.erase(line.size() - 1);

			buf_ref.erase(0, pos + 1);

			if (line.empty())
				continue;

			std::vector<std::string> tokens = CmdFactory::splitBySpaces(line);
			if (tokens.empty())
				continue;

			std::string cmd_name = tokens[0];

			// Capizalize user input: e.g., join -> JOIN
			for (size_t i = 0; i < cmd_name.size(); ++i)
				cmd_name[i] = std::toupper(cmd_name[i]);

			std::cout << "cmd = " << cmd_name << std::endl;

			User &user = users_[user_fd];
			if (!user.isRegistered() &&
				cmd_name != "PASS" && cmd_name != "NICK" &&
				cmd_name != "USER" && cmd_name != "QUIT" &&
				cmd_name != "PING" && cmd_name != "PONG") {
				std::string nick = user.getNickname().empty() ? "*" : user.getNickname();
				std::string errMsg = ":" + this->getServerName() + " 451 " + nick + " :You have not registered";
				users_[user_fd].reply(errMsg);
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
				std::string errMsg = ":" + this->getServerName() + " 421 " + nick + " " + cmd_name + " :Unknown command";
				user.reply(errMsg);
			}

			// Exit if the user has been removed via QUIT command
			if (this->users_.find(user_fd) == this->users_.end()) {
				return;
			}
		}
	} else if (bytes_read == 0) {
		std::cout << "Client disconnected: " << user_fd << std::endl;
		removeUser(i);
	} else {
		// EWOULDBLOCK and EAGAIN => No data received
		if (errno == EWOULDBLOCK || errno == EAGAIN)
			return;
		else
			removeUser(i);
	}
}

// Remove a user from the server using an its index
void Server::removeUser(int i) {
	int fd = user_poll_[i].fd;
	UserMap::iterator it = users_.find(fd);

	if (it != users_.end()) {
		User &user = it->second;
		std::map<std::string, Channel*> joinedChannels = user.getJoinedChannels();

		// remove user from the joined channels
		for (std::map<std::string, Channel*>::iterator chan_it = joinedChannels.begin(); chan_it != joinedChannels.end(); ++chan_it) {
			Channel* joined_Channel = chan_it->second;

			joined_Channel->removeUser(&user);

			// If the user is the last member in the channel, remove the channel
			if (joined_Channel->getMemberCount() == 0)
				this->removeChannel(joined_Channel->getChannelName());
		}
	}

	close(fd);
	users_.erase(fd);

	// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
	// If the user being removed is not the latest user, overwrite its slot with the latest user in user_poll_
	if (i < (int)fd_count_ - 1)
		user_poll_[i] = user_poll_[fd_count_ - 1];

	// If the user being removed is the latest user, the last user socket is ignored by the code line below
	fd_count_--;
	// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	// => With this code, it doesn't need to shift the remaming elements one by one -> Server effectivity

	std::cout << "[Server] User removed. Current count: " << fd_count_ << std::endl;
}

// Remove a user from the server using a User object
void Server::removeUser(User &user) {
	int fd = user.getFd();

	for (int i = 0; i < fd_count_; i++) {
		if (user_poll_[i].fd == fd) {
			this->removeUser(i);
			return;
		}
	}
}

// Remove a channel from the server using the channel name
void	Server::removeChannel(std::string channel_name) {
	ChannelMap::iterator it = channels_.find(channel_name);
	if (it != channels_.end()) {
		delete it->second;
		channels_.erase(it);
	}
}

// Start the main server event loop and handle socket events
void Server::run() {
	// SIGINT: Ctrl+C, signalHandler: callback function
	signal(SIGINT, signalHandler);

	setSocket();

	while (g_server_run) {
		// Block and wait for events on monitored sockets
		if (poll(user_poll_, fd_count_, -1) < 0) {
			// Resume poll if interrupted by a signal (EINTR)
			if (errno == EINTR) // Prevent poll failure by Ctrl+C
				continue;
			throw Server::RunTimeError("Error: poll failed.");
		}

		for (int i = 0; i < fd_count_; i++) {
			if (user_poll_[i].revents & POLLIN) {
				if (user_poll_[i].fd == server_fd_) {
					newClient();
				} else {
					int count_before = fd_count_;
					clientRequest(i);

					// Decrement index to prevent skipping a slot if a user was removed
					if (fd_count_ < count_before) {
						i--;
						continue;
					}
				}
			} else if (user_poll_[i].revents & (POLLHUP | POLLERR)) {
				// Handle client disconnection or socket error events
				removeUser(i);
				i--;
				continue;
			}
			// Clear event flags to prepare for the next poll iteration
			user_poll_[i].revents = 0;
		}
	}
}
