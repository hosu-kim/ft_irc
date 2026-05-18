/**
 * Command: PART
 * Description: Used to leave one or more channels that the user has joined.
 *              If a message is provided, it'll be sent to the channel members.
 *              This command allows a user to depart from channels.
 * 
 * Syntax: PART <channel>[,<channel>] [<message>]
 * 
 * Examples:
 *    PART #chatroom
 *    PART #chatroom,#other :Goodbye everyone!
 * 
 * Note: If the user is not a member of a specified channel, the server should
 *       respond with an appropriate error (ERR_NOTONCHANNEL).
 */

#pragma once

#include "ACmd.hpp"

class CmdPart : public ACmd {
	public:
		CmdPart(const std::string &cmd, const std::vector<std::string> &params)
		: ACmd(cmd, params) {}

		virtual ~CmdPart() {}

		// Helper
		virtual std::string join_params_from_index(std::size_t start) const;
		// Core function
		virtual void execute(User &user, Server &server);
};