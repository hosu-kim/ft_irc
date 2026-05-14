/**
 * Command: INVITE
 * Description: Invites the given user to the given channel.
 *              It is especially usuful for allowing users to join
 *              invite-only channels (mode +i).
 * 
 * syntax: INVITE <nickname> <channel>
 * 
 * Example: INVITE friend123 #42prague
 */

#pragma once

#include "ACmd.hpp"

class Server;
class User;

class CmdInvite : public ACmd
{
	public:
		CmdInvite(std::string cmd, std::vector<std::string> params)
		: ACmd(cmd, params) {}
		virtual ~CmdInvite() {}

		/* LOGIC FUNCTIONS */
		virtual void execute(User &user, Server &server);
};