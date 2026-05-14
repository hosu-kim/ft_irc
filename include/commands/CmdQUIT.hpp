/**
 * Command: QUIT
 * Description: Disconnects the user from the server.
 *              An optional quit message can be provided, which is broadcasted to
 *              other user in the channels that the quitting user is a member of.
 * 
 * Syntax: QUIT [:<Quit message>]
 * 
 * Examples:
 *    Case 1. With message: QUIT :Bye everyone, leaving for dinner!
 *    Case 2. Without message: QUIT
 */

#pragma once

#include "ACmd.hpp"

class CmdQuit : public ACmd
{
	public:
		CmdQuit(std::string cmd, std::vector<std::string> params)
		: ACmd(cmd, params) {}

		virtual ~CmdQuit() {}

		virtual void execute(User &user, Server &server);
};