#pragma once

#include "ACmd.hpp"
#include <sstream>

class CmdMode : public ACmd {
	public:
		CmdMode(const std::string &cmd, const std::vector<std::string> &params)
		: ACmd(cmd, params) {}

		virtual ~CmdMode() {}

		virtual void execute(User &user, Server &server);
};