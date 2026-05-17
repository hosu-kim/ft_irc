#pragma once

#include "ACmd.hpp"

class CmdPart : public ACmd {
	public:
		CmdPart(const std::string &cmd, const std::vector<std::string> &params)
		: ACmd(cmd, params) {}

		virtual ~CmdPart() {}

		virtual void execute(User &user, Server &server);
};