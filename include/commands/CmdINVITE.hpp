/* 
INVITE: Invites the given user to the given channel.

Syntax: INVITE <nickname> <channel>

example: INVITE friend123 #42prague
*/

#pragma once



#include "ACmd.hpp"

class CmdInvite : public ACmd
{
	public:
		CmdInvite(std::string cmd, std::vector<std::string> params)
		: ACmd(cmd, params) {}

		virtual ~CmdInvite() {}

		virtual void execute(User &user, Server &server);
};