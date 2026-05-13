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
	/*
	 * 1. Check parameter count
	 * Syntax: PASS <password>
	 */
	if (getParamCount() < 1) {
		std::string nick = user.getNickname().empty() ? "*" : user.getNickname();
		std::string msg = ":" + server.getServerName() + " 461 " + nick + " PASS: Not enough parameters";
		user.reply(msg);
		return;
	}

	/*
	 * 2. Check if the user is already registered
	 */
	if (user.getRegistered()) {
		std::string nick = user.getNickname().empty() ? "*" : user.getNickname();
		std::string msg = ":" + server.getServerName() + " 462 " + nick + " :Unauthorized command (already registered)";
		user.reply(msg);
		return;
	}

	/*
	* 3. Validate the password
	*/
	std::string given_password = this->_params[0];
	if (given_password == server.getPassword()) {
		user.setPassOK(true);
}