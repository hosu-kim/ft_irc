/**
 * Command: PING
 * Description: Used by the server and client to check if the connection is still alive.
 *              When a PING is received, a PONG response must be sent back immediately.
 * 
 * Syntax: PING <server/client_name>
 * 
 * Examples:
 *    Case 1. Client-side (Client initiates)
 *            - Client sends: PING :hello
 *            - Server sends back: :srv123 PONG srv123 :hello
 * 
 *    Case 2. Server-side (Server initiates)
 *            - Server sends: PING :srv123
 *            - Client sends back: PONG :srv123
 */

#include "CmdPING.hpp"

void CmdPing::execute(User& user, Server& server) {
	/*
	 * Check if the params container is empty
	 */
	if (getParamCount() == 0) {
		std::string errMsg = ":" + server.getServerName() + " 409 " + user.getUserName() + " :No origin specified";
		user.reply(errMsg);
		return;
	}

	/*
	 * Grab the token sent by the client
	 */
	std::string received_token = _params[0];

	/*
	 * Build the PONG response message
	 */
	std::string response = ":" + server.getServerName() + " PONG " + server.getServerName() + " :" + received_token;

	/*
	 * Send it back to the client
	 */
	user.reply(response);
}