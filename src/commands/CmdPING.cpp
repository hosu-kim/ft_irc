#include "CmdPING.hpp"

void CmdPing::execute(User& user, Server& server) {
	/*
	Check if the params container is empty
	*/
	if (getParamCount() == 0) {
		std::string nick = user.getNickname().empty() ? "*" : user.getNickname();
		std::string msg = ":" + server.getServerName() + " 409 " + nick + " :No origin specified";
		user.reply(msg);
		return;
	}

	/*
	Grab the token sent by the client
	*/
	std::string received_token = _params[0];

	/*
	Build the PONG response message
	*/
	std::string msg = ":" + server.getServerName() + " PONG " + server.getServerName() + " :" + received_token;

	/*
	Send it back to the client
	*/
	user.reply(msg);
}