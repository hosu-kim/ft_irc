/**
 * Command: PASS
 * Description: Used to set a connection password.
 *              In the IRC protocol, this command MUST be sent before
 *              the NICK and USER commands during the initial connection.
 * 
 * Syntax: PASS <password>
 * 
 * Example: PASS pw123
 */

#pragma once

#include "ACmd.hpp"

class CmdPass : public ACmd {
	public:
		CmdPass(std::string cmd, std::vector<std::string> params)
		: ACmd(cmd, params) {}
		virtual ~CmdPass() {}

		virtual void execute(User &user, Server &server);
};
