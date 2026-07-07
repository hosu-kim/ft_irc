/**
 * Command: KICK
 * Description: Used when a channel operator forcefully removes (=kicks)
 *              a user from a channel.
 * 
 * Parmas: 0    1         3      4
 * Syntax: KICK <channel> <user> [<reason>]
 * 
 * Example: KICK #42prague bad_user :Don't spam!
 */

#pragma once

#include "ACmd.hpp"

class CmdKick : public ACmd {
	public:
		CmdKick(std::string cmd, std::vector<std::string> params)
		: ACmd(cmd, params) {}
		virtual ~CmdKick() {}

		virtual void execute(User &user, Server &server);
};