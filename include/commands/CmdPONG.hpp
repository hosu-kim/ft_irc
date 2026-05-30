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

#pragma once

#include "ACmd.hpp"

class CmdPong : public ACmd {
	public:
		CmdPong(const std::string& cmd, const std::vector<std::string>& params)
		: ACmd(cmd, params) {}
		virtual ~CmdPong() {}

		virtual void execute(User &user, Server& server);
};
