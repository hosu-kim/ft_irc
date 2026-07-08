/**
 * Command: PASS
 * Description: Used to set a connection password.
 *              In the IRC protocol, this command MUST be sent before
 *              the NICK and USER commands during the initial connection.
 * 
 * Params: 0    1
 * Syntax: PASS <password>
 * 
 * Example: PASS pw123
 */

#include "CmdPASS.hpp"

void CmdPass::execute(User &user, Server &server) {
	// "*" used as a temporary placeholder if the user hasn't set a nickname yet.
	// This prevents formatting errors in IRC numeric replies. (=> std::string msg = ...)
	std::string nick = user.getNickname().empty() ? "*" : user.getNickname();

	// Parameter validation
	if (getParamCount() < 1) {
		std::string err_msg = ":" + server.getServerName() + " 461 " + nick + " PASS: Not enough parameters";
		user.reply(err_msg);
		return;
	}

	// Check if the user is already registered
	if (user.isRegistered()) {
		std::string err_msg = ":" + server.getServerName() + " 462 " + nick + " :Unauthorized command (already registered)";
		user.reply(err_msg);
		return;
	}

	// Validate the password
	std::string given_password = this->getParam(0);
	if (given_password == server.getPassword()) {
		user.setPassOK(true);
	} else {
		std::string err_msg = ":" + server.getServerName() + " 464 " + nick + " :Password incorrect";
		user.reply(err_msg);
	}
}