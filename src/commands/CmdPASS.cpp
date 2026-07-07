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

#include "CmdPASS.hpp"

void CmdPass::execute(User &user, Server &server) {
	// "*" used as a temporary placeholder if the user hasn't set a nickname yet.
	// This prevents formatting errors in IRC numeric replies. (=> std::string msg = ...)
	std::string nick = user.getNickname().empty() ? "*" : user.getNickname();

	/*
	 * 1. Check parameter count
	 * Syntax: PASS <password>
	 */
	if (getParamCount() < 1) {
		std::string errMsg = ":" + server.getServerName() + " 461 " + nick + " PASS: Not enough parameters";
		user.reply(errMsg);
		return;
	}

	/*
	 * 2. Check if the user is already registered
	 */
	if (user.isRegistered()) {
		std::string errMsg = ":" + server.getServerName() + " 462 " + nick + " :Unauthorized command (already registered)";
		user.reply(errMsg);
		return;
	}

	/*
	 * 3. Validate the password
	 */
	std::string given_password = this->params_[0];
	if (given_password == server.getPassword()) {
		user.setPassOK(true);
	} else {
		std::string errMsg = ":" + server.getServerName() + " 464 " + nick + " :Password incorrect";
		user.reply(errMsg);
	}
}