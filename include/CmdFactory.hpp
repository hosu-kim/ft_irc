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

#include "ACmd.hpp"
#include "CmdJOIN.hpp"

class ACmd;

class CmdFactory {
	private:
		CmdFactory();
	public:
		static std::vector<std::string> splitBySpaces(std::string str);
		static ACmd* createCommand(std::vector<std::string> tokens);

};