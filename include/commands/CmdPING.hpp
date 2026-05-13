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

#pragma once

#include "ACmd.hpp"

class CmdPing: public ACmd {
	public:
		CmdPing(const std::string& cmd, const std::vector<std::string>& params) {}
		virtual ~CmdPing() {}

		virtual void execute(User &user, Server& server);
};
