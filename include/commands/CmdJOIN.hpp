/* 
JOIN: used when user joins a channel
Syntax: JOIN <channel> [<password>]
Example: Case 1. Join a channel with NO PASSWORD: JOIN #42prague
         Case 2. Join a channel with PASSWORD: JOIN #42prague password123
*/

#pragma once

#include "ACmd.hpp"

class Server;
class User;

class CmdJoin : public ACmd
{
	public:
		CmdJoin(std::string cmd, std::vector<std::string> params)
		: ACmd(cmd, params) {}

		virtual ~CmdJoin() {}

		virtual void execute(User &user, Server &server);
};