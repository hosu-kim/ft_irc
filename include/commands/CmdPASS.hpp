/* PASS: This command is used to set a connection password.

	Syntax: PASS <password>

	Example: PASS pw123
*/
#pragma once

#include "ACmd.hpp"

class CmdPass : public ACmd {
	public:
		CmdPass(std::string cmd, std::vector<std::string> params) {}
		virtual ~CmdPass() {}

		virtual void execute(User &user, Server &server);
};
