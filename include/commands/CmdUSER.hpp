/**
 * Command: USER
 * Description: Used at the beginning of a connection to specify the username,
 *              hostname, servername, and realname of a new user.
 *              NICK command is required to register a connection.
 *              Once a user is registered, using this command again will return an error.
 * 
 * Syntax: USER <username> <hostname> <servername> :<realname>
 * 
 * Examples:
 *    Case 1. Typical registration: USER guest 0 * :John Smith
 *    Case 2. Full details: USER jsmith localhost irc.server.com :John Smith
 */

#pragma once

#include "ACmd.hpp"

class CmdUser : public ACmd {
	public:
		CmdUser(std::string cmd, std::vector<std::string> params)
		: ACmd(cmd, params) {}

		virtual ~CmdUser() {}

		virtual void execute(User &user, Server &server);
};