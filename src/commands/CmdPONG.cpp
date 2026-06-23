/**
 * Command: PONG
 * Description: Sent in response to a PING to indicate that the connection is still alive.
 *              Clients reply with PONG when they receive a server PING, and the server
 *              may also send PONG replies in certain cases. No further automatic reply
 *              is required upon receiving a PONG.
 *
 * Syntax: PONG <server/client_name> [:message]
 *
 * Examples:
 *    Case 1. Client responding to server PING
 *            - Server sends: PING :srv123
 *            - Client sends back: PONG :srv123
 *
 *    Case 2. Server responding to client-initiated PING
 *            - Client sends: PING :hello
 *            - Server sends back: :srv123 PONG srv123 :hello
 */

#include "CmdPONG.hpp"

void CmdPong::execute(User &user, Server &server) {
	/*
	 * 1. Parameter Validation:
	 *    If no parameters are provided, return ERR_NOORIGIN (409).
	 *    Syntax: PONG <server1> [<server2>]
	 */
	if (getParamCount() == 0) {
		std::string errMsg = ":" + server.getServerName() + " 409 " + user.getNickname() + " :No origin specified";
		user.reply(errMsg);
		return;
	}

	/*
	 * 2. Update Last Activity:
	 *    Receiving PONG indicates the client is still alive.
	 *    Update the user's last activity timestamp to prevent Ping-timeout.
	 */
	user.updateActivity();

	/*
	 * 3. No further reply:
	 *    No additional message is required to be sent back in response to a PONG.
	 */
	return;
}
