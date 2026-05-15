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
	// (Assuming user.getUserName() returns an empty string if not set yet).
	if (user.getUserName().empty() == false) {
		std::string errMsg = ":" + server.getServerName() + " 462 " + nick + " :Unauthorized command (already registered)";
		user.reply(errMsg);
		return;
	}

	/* 
	 * 3. Save the user information
	 */
	std::string userName = this->getParam(0);
	// param(1) and param(2) are usually '0' and '*' (ignored).
	std::string realName = this->getParam(3);

	user.setUserName(userName);
	user.setRealName(realName);

	/* 
	 * 4. Check if Registration is complete
	 */
	// A client is fully registered and allowed to chat only after they successfully 
	// provide BOTH the NICK and USER commands (and PASS if the server has a password).
	// Because NICK and USER can arrive in any order, we must check it here.
	if (user.getNickname().empty() == false && user.getPassOK()) {
		
		// 4a. Mark the user as fully registered in the server
		user.setRegistered(true);

		// 4b. Send the mandatory Welcome sequence (001)
		std::string welcomeMsg = ":" + server.getServerName() + " 001 " + nick + " :Welcome to the ft_irc Network, " + nick + "!" + userName + "@" + user.getIpAddress();
		user.reply(welcomeMsg);
		// Send the mandatory Welcome sequence (001, 002, 003, 004)
		// std::string welcomeMsg = ":" + server.getServerName() + " 001 " + nick + " :Welcome to the ft_irc Network, " + nick + "!" + userName + "@" + user.getHostName();
		// user.reply(welcomeMsg);
		
		// (Usually, servers also send RPL_YOURHOST (002), RPL_CREATED (003), RPL_MYINFO (004))
		// server.sendWelcomeMessages(user);
	}
}