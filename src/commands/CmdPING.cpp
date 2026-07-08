/**
 * Command: PING
 * Description: Used by the server and client to check if the connection is still alive.
 *              When a PING is received, a PONG response must be sent back immediately.
 * 
 * Params:      0
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
	// Parameter validation
	if (this->getParamCount() == 0) {
		std::string err_msg = ":" + server.getServerName() + " 409 " + user.getUserName() + " :No origin specified";
		user.reply(err_msg);
		return;
	}

	// Grab the message sent by the client
	std::string received_token = this->params_[0];

	// Build the PONG response message
	std::string response_msg = ":" + server.getServerName() + " PONG " + server.getServerName() + " :" + received_token;

	// * Send it back to the client
	user.reply(response_msg);
}