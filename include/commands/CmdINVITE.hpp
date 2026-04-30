/* 
INVITE: Invites the given user to the given channel.

Syntax: INVITE <nickname> <channel>

example: INVITE friend123 #42prague
*/

#pragma once

#include "ACmd.hpp"

class Server;
class User;

class CmdInvite : public ACmd
{
	public:
		/* ORTHODOX CANONICAL FORM */
		CmdInvite();
		CmdInvite(std::string cmd, std::vector<std::string> params);
		CmdInvite(const CmdInvite& src);
		CmdInvite& operator=(const CmdInvite& src);
		// REMINDER:
		// if you declare virtual once  in the parent class,
		// the children automatically become virutal.
		// However, it's best practice to use virtual in the child class
		// as well to explicitly show that this class is in an inheritance structure.
		// vvv
		virtual ~CmdInvite();

		/* LOGIC FUNCTIONS */
		virtual void execute(User &user, Server &server);
};