/**
 * Command: PRIVMSG
 * Description: Sends a message to a specific user or a channel.
 *              Despite the name "PRIV", it is used for both 1:1 private messages
 *              and public channel messages.
 * 
 * Syntax: PRIVMSG <target> :<message>
 * 
 * Examples:
 *    Case 1. Channel message: PRIVMSG #42prague :Hello everyone!
 *    Case 2. Private message: PRIVMSG friend123 :Hi there!
 */

#include "CmdPRIVMSG.hpp"

void CmdPrivmsg::execute(User &user, Server &server) {
	// "*" used as a temporary placeholder if the user hasn't set a nickname yet.
	// This prevents formatting errors in IRC numeric replies. (=> std::string msg = ...)
	std::string nick = user.getNickname().empty() ? "*" : user.getNickname();

	/*
	 * 1. Check parameter count
	 */
	// 1a. If no parameters given (= 0):
	if (this->getParamCount() == 0) {

		std::string errMsg = ":" + server.getServerName() + " 411 " + nick + " :No recipient given";
		user.reply(errMsg);
		return;
	}

	// 1b. If only 1 parameter given (= has target, but no message):
	if (this->getParamCount() == 1) {
		std::string errMsg = ":" + server.getServerName() + " 412 " + nick + " :No text to send";
		user.reply(errMsg);
		return;
	}

	/*
	 * 2. Extract target and chat message
	 */
	std::string target = this->getParam(0);
	std::string chatMsg = this->getParam(1);

	/*
	* 3. Route the message: Is it a Channel or a User?
	*/

	// 3a. If target starts with '#' (= target is a CHANNEL => send chatMsg to channel)
	if (target[0] == '#') {
		// 3a-1. Find the channel in the server. 
		Channel* targetChannel = server.getChannel(target);

		// 3a-2. If the channel does not exist:
		if (targetChannel == NULL) {
			std::string errMsg = ":" + server.getServerName() + " 403 " + nick + " " + target + " :Channel does not exist";
			user.reply(errMsg);
			return;
		}

		// 3a-3. If the sender (user) is NOT in that channel:
		if (targetChannel->getUserByNick(user.getNickname()) == NULL) {
			std::string errMsg = ":" + server.getServerName() + " 404 " + nick + " " + target + " :User is not in channel";
			user.reply(errMsg);
			return;
		}
		// 3a-4. Else👌: All checks passed => send chatMsg to the channel
		else {
			std::string formattedChatMsg = ":" + user.getNickname() + "!"
										 + user.getUserName() + "@"
										 + user.getHostName() + " PRIVMSG "
										 + target + " :" + chatMsg;
			targetChannel->broadcast(formattedChatMsg, &user);
		}
	} 
	// 3b. if target is a user => send chatMsg to the user.
	else {
		// 3b-1. Find the user in the server by their nickname.
		User* targetUser = server.getUserByNick(target);

		// 3b-2. If the user does not exist:
		if (targetUser == NULL) {
			std::string errMsg = ":" + server.getServerName() + " 401 " + nick + " :No such nick";
			user.reply(errMsg);
			return;
		}
		// 3b-3. Else👌: All checks passed => send chatMsg to the user
		else {
			//      Send the message directly to that targetUser
			// ":userNick!username@hostName PRIVMSG targetUser :Hello"
			std::string formattedChatMsg = ":" + user.getNickname() + "!"
										 + user.getUserName() + "@"
										 + user.getHostName() + " PRIVMSG "
										 + targetUser->getNickname() + " :" + chatMsg;
			targetUser->reply(formattedChatMsg);
		}
	}
}