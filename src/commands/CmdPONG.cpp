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
		// std::string errMsg = ":" + server.getServerName() + " 409 " + user.getNickname() + " :No origin specified";
		// user.reply(errMsg);
		return;
	}

	/*
	 * 2. Target Server Check (Optional):
	 *    If a second parameter <server2> is provided, the message should be forwarded there.
	 *    In a local server environment, this can be ignored or used to verify the server existence.
	 */

	/*
	 * 3. Prevent User Timeout:
	 *    By receiving PONG, the server acknowledges that the connection is still alive.
	 *    Typically, update the user's last activity time to prevent disconnects due to Ping-timeout.
	 */

	// TODO: Add logic to update user's last activity time
}
	 * 2. Parse target token and optional trailing message
	 */
	std::string token = this->getParam(0);
	if (token[0] == ':')
		token = token.substr(1);
	
	std::string message = "";
	if (this->getParamCount() > 1) {
		message = this->combine_params_with_spaces(1);
		if (!message.empty() && message[0] == ':') message = message.substr(1);
	}

/*
3. // Update keep-alive / ping state
	// Record that this user responded to a ping
	user.setLastPongTime(now())      // or store a timestamp field

	// If server was waiting for a PONG from this user:
	if server.hasPendingPingFor(user):
		 expectedToken = server.getPendingPingToken(user)
		 if expectedToken == token:
			  // Optionally compute RTT if server stored ping timestamp
			  sentAt = server.getPendingPingTimestamp(user)
			  rtt = now() - sentAt
			  user.setPingLatency(rtt)    // optional
			  server.clearPendingPingFor(user)
*/

/*
4. // Logging / monitoring (optional)
	server.log("PONG received from " + user.getUserName() + " token=" + token + " msg=" + message)

5. // No reply should be sent in response to a PONG
	return
*/

}