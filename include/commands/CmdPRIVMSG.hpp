/**
 * Command: PRIVMSG
 * Description: Sends a message to a specific user or a channel.
 *              Despite the name "PRIV", it is used for both 1:1 private messages
 *              and public channel messages.
 * 
 * Syntax: PRIVMSG <target> :<message>
 * 
 * Examples:
 *    Case 1. Channel message: PRIVMSG #42prague :Hello everyone!
 *    Case 2. Private message: PRIVMSG friend123 :Hi there!
 */

#pragma once

#include "ACmd.hpp"

class CmdPrivmsg: public ACmd {
	public:
		CmdPrivmsg(const std::string &cmd, const std::vector<std::string> &params)
		: ACmd(cmd, params) {}
		virtual ~CmdPrivmsg() {}
	
		virtual void execute(User &user, Server &server);
};