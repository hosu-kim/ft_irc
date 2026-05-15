/**
 * Command: USER
 * Description: Used at the beginning of a connection to specify the username,
 *              hostname, servername, and realname of a new user.
 *              It must be used alongside the NICK command to register a connection.
 *              Once a user is registered, using this command again will return an error.
 * 
 * Syntax: USER <username> <hostname> <servername> :<realname>
 * 
 * Examples:                                               vvv: <hostname> <servername> => No longer used
 *    Case 1. Typical registration (new usage): USER guest 0 * :John Smith
 *    Case 2. Full details (old usage): USER jsmith localhost irc.server.com :John Smith
 * 
 * Note: The <hostname> and <servername> parameters are no longer used these days,
 *       so modern clients typically send '0' and '*' as placeholders
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