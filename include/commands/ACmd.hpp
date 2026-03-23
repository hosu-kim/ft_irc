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
#include <vector>

#include "User.hpp"
#include "Server.hpp"
//#include "CmdFactory.hpp"

// Parser logic:
// DONE 1. Split by spaces
//      2. If ":" appears:
//            -> everything after = ONE param
//      Example:
//          Input: USER test 0 * :Real Name
//          Output: ["USER", "test", "0", "*", "Real Name"]
// Commands to handle:
//      Priority:
//          PASS, NICK, USER
//      After:
//          PRIVMSG
//      Last:
//          JOIN

// Example: PRIVMSG #chan :hello world

class Server;

class ACmd
{
	private:
		// e.g., KICK #channel target_user
		//       cmd: "NICK"
		std::string					_cmd;
		//       params: ["#channel", "target_user"]
		std::vector<std::string>	_params;
		ACmd();

	protected:
		// child class uses this parameterized constructor.
		ACmd(std::string cmd, std::vector<std::string> params);

		// getters
		const std::string& getParam(size_t index);
		size_t getParamCount() const;

	public:
		// No implementation of the copy constructor and copy assignment constructor.
		// they are not used in abstract classes.
		ACmd(const ACmd& other);
		ACmd& operator=(const ACmd& ohter);
		virtual ~ACmd();

		// Pure Virtual Function: Child classes implement this funciton themselves :)
		virtual void execute(User &user, Server &server) = 0;

		// => this function to the CmdFactory as well
		// static std::vector<std::string> splitBySpaces(std::string str); // is static necessary?

		// => Hosu: better to implement this function below in the CmdFactory class
		//          because ACmd only represents the parent of each child cmd class
};