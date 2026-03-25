/* NICK: used when user sets up a nickname

   Syntax: NICK <nickname>

   Example: NICK a_cool_nick
*/
#pragma once

#include "ACmd.hpp"
#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"

#define FORBIDDEN_CHARS "!@#$%^&*()+={}[];:',\"\t<>"

class CmdNick : public ACmd
{
	public:
		CmdNick(std::string cmd, std::vector<std::string> params)
		: ACmd(cmd, params) {}

		virtual ~CmdNick() {}

		virtual void execute(User &user, Server &server);
};