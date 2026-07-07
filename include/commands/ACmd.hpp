#pragma once

#include <string>
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
#include <vector>

#include "../User.hpp"
#include "../Server.hpp"

class Server;
class User;
class Channel;

class ACmd {
protected:
	std::string cmd_;
	std::vector<std::string> params_;
	ACmd();
	// No implementation of the copy constructor and copy assignment constructor
	// not used in abstract classes.

	// Child classes uses only this parameterized constructor.
	ACmd(std::string cmd, std::vector<std::string> params);
	ACmd(const ACmd& other);
	ACmd& operator=(const ACmd& ohter);

public:
	virtual ~ACmd();

	/* GETTTERS */
	const std::string& getParam(size_t index) const;
	size_t getParamCount() const;

	/* HELPERS */
	virtual std::string combineParamsToOneStr(std::size_t start) const;
	virtual std::vector<std::string> splitStr(char delimiter) const;

	virtual void execute(User &user, Server &server) = 0;
};