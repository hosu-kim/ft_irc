/**
 * Command: USER
 * Description: Used at the beginning of a connection to specify the username,
 *              hostname, servername, and realname of a new user.
 *              It must be used alongside the NICK command to register a connection.
 *              Once a user is registered, using this command again will return an error.
 * 
 * Params:      0          1          2             3
 * Syntax: USER <username> <hostname> <servername> :<realname>
 * 
 * Examples:                                               vvv: <hostname> <servername> => No longer used
 *    Case 1. Typical registration (new usage): USER guest 0 * :John Smith
 *    Case 2. Full details (old usage): USER jsmith localhost irc.server.com :John Smith
 * 
 * Note: The <hostname> and <servername> parameters are no longer used these days,
 *       so modern clients typically send '0' and '*' as placeholders.
 */

#include "CmdUSER.hpp"

void CmdUser::execute(User &user, Server &server) {
	// "*" used as a temporary placeholder if the user hasn't set a nickname yet.
	// This prevents formatting errors in IRC numeric replies. (=> std::string msg = ...)
	std::string nick = user.getNickname().empty() ? "*" : user.getNickname();

	// Parameter validation: USER command strictly requires exactly 4 parameters.
	if (this->getParamCount() < 4) {
		std::string err_msg = ":" + server.getServerName() + " 461 " + nick + " USER :Not enough parameters";
		user.reply(err_msg);
		return;
	}

	// Check if the user is already registered
	if (user.isRegistered() || !user.getUserName().empty()) {
		std::string err_msg = ":" + server.getServerName() + " 462 " + nick + " :Unauthorized command (already registered)";
		user.reply(err_msg);
		return;
	}

	// Save the user information
	// Note: param(1) <hostname> and param(2) <servername> are not used in modern IRC
	std::string user_name = this->getParam(0);
	std::string real_name = this->getParam(3);

	user.setUserName(user_name);
	user.setRealName(real_name);
	user.setHasUser(true);

	// if PASS, NICK, and USER have been validated
	if (user.getHasNick() && user.getHasUser() && user.getPassOK()) {
		user.setRegistered(true);

		std::string welcome_msg = ":" + server.getServerName() + " 001 " + nick + " :Welcome to the ft_irc Network, " + nick + "!" + user_name + "@" + user.getHostName();
		user.reply(welcome_msg);
	}
}