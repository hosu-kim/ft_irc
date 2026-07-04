/**
 * Command: USER
 * Description: Used at the beginning of a connection to specify the username,
 *              hostname, servername, and realname of a new user.
 *              It must be used alongside the NICK command to register a connection.
 *              Once a user is registered, using this command again will return an error.
 * 
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

	/* 
	 * 1. Check if there are enough parameters
	 */
	// USER command strictly requires exactly 4 parameters.
	if (this->getParamCount() < 4) {
		std::string errMsg = ":" + server.getServerName() + " 461 " + nick + " USER :Not enough parameters";
		user.reply(errMsg);
		return;
	}

	/* 
	 * 2. Check if the user is already registered
	 */
	// If the user's username is already set, they cannot call the USER command again.
	if (user.isRegistered() || !user.getUserName().empty()) {
		std::string errMsg = ":" + server.getServerName() + " 462 " + nick + " :Unauthorized command (already registered)";
		user.reply(errMsg);
		return;
	}

	/* 
	 * 3. Save the user information
	 */
	// Note: param(1) <hostname> and param(2) <servername> are ignored in modern IRC
	std::string userName = this->getParam(0);
	std::string realName = this->getParam(3);

	user.setUserName(userName);
	user.setRealName(realName);
	user.setHasUser(true);

	/* 
	 * 4. Check if Registration is complete
	 */
	// Connection requires PASS, NICK, and USER to be validated
	if (user.getHasNick() && user.getHasUser() && user.getPassOK()) {
		
		// 4a. Mark the user as fully registered in the server
		user.setRegistered(true);

		// 4b. Send the mandatory Welcome message (001)
		std::string welcomeMsg = ":" + server.getServerName() + " 001 " + nick + " :Welcome to the ft_irc Network, " + nick + "!" + userName + "@" + user.getHostName();
		user.reply(welcomeMsg);
	}
}