/**
 * Command: JOIN
 * Description: Used when a user wants to join a specific channel.
 *              If the channel does not exist, it will be created automatically.
 * 
 * Syntax: JOIN <channel> [<password>]
 * 
 * Examples:
 *    Case 1. Join a channel with NO PASSWORD: JOIN #42prague
 *    Case 2. Join a channel with PASSWORD: JOIN #42prague password123
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