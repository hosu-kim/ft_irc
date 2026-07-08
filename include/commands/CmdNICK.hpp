/**
 * Command: NICK
 * Description: Used to set or change the user's nickname.
 *              It is one of the essential commands for a user to register
 *              and be recognized by the IRC server.
 * 
 * Params: 0    1
 * Syntax: NICK <nickname>
 * 
 * Example: NICK a_cool_nick
 */

#pragma once

#include "ACmd.hpp"
#define FORBIDDEN_CHARS "!@#$%^&*()+={}[];:',\"\t<>"

class CmdNick : public ACmd {
	public:
		CmdNick(const std::string &cmd, const std::vector<std::string> &params)
		: ACmd(cmd, params) {}

		virtual ~CmdNick() {}

		virtual void execute(User &user, Server &server);
};