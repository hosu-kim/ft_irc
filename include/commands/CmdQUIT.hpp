#pragma once

#include "ACmd.hpp"
#include "Server.hpp"
#include "User.hpp"
#include "Channel.hpp"


class CmdQuit : public ACmd
{
	public:
		CmdQuit(std::string cmd, std::vector<std::string> params)
		: ACmd(cmd, params) {}

		virtual ~CmdQuit() {}

		virtual void execute(User &user, Server &server);
};