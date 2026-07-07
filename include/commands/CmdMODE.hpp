/**
 * Command: MODE
 * Description: Used to change or query the active modes of a channel.
 *              Only channel operators cna modify modes (i, t, k, l, o).
 *              If no mode parameters are provided, it queries the active modes.
 * 
 * Params" 0    1         2                 3
 * Syntax: MODE <channel> [[+/-]<mode_char> [<mode_arg>]]
 * 
 * Examples:
 *    Case 1. Query active modes: MODE #42prague
 *    Case 2. Toggle invite-only: MODE #42prague +i
 *    Case 3. Set topic restriction: MODE #42prague +t
 *    Case 4. Set channel password: MODE #42prague +k password123
 *    Case 5. Limit member count: MODE #42prague +l 10
 *    Case 6. Toggle operator privileges: MODE #42prague +o user123
 */

#pragma once

#include "ACmd.hpp"
#include <sstream>

class CmdMode : public ACmd {
	public:
		CmdMode(const std::string &cmd, const std::vector<std::string> &params)
		: ACmd(cmd, params) {}

		virtual ~CmdMode() {}

		virtual void execute(User &user, Server &server);
};