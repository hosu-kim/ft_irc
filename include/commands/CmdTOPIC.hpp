/**
 * Command: TOPIC
 * Description: Used to change or view the topic of a channel.
 *              If a topic parameter is not provided, it returns the current topic of the channel.
 *              If provided, it sets the channel's topic to the new value.
 *              To clear a channel's topic, an empty topic string can be provided.
 * 
 * Syntax: TOPIC <channel> [<topic>]
 * 
 * Examples:
 *    Case 1. View topic: TOPIC #channel
 *    Case 2. Set topic: TOPIC #channel :This is the new channel topic!
 *    Case 3. Clear topic: TOPIC #channel :
 */

#pragma once

#include "ACmd.hpp"

class CmdTopic : public ACmd {
	public:
		CmdTopic(std::string cmd, std::vector<std::string> params)
		: ACmd(cmd, params) {}

		virtual ~CmdTopic() {}

		virtual void execute(User &user, Server &server);
};