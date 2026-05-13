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
	// If no parameters given (= 0):
	if (this->getParamCount() == 0) {

		std::string errMsg = ":" + server.getServerName() + " 411 " + nick + " :No recipient given";
		user.reply(errMsg);
		return;
	}

	// If only 1 parameter given (= has target, but no message):
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
	// If target starts with '#' (= target is a channel):
	
	//         - Find the channel in the server by its name.
	//         - IF the channel does not exist:
	//              SEND ERR_NOSUCHNICK (401) -> "No such nick/channel"
	//              RETURN
	//         - IF the user is NOT in that channel (Cannot send messages to a channel you haven't joined!):
	//              SEND ERR_CANNOTSENDTOCHAN (404) -> "Cannot send to channel"
	//              RETURN
	//         - ELSE:
	//              Broadcast the message to everyone in the channel EXCEPT the sender!
	
	//    - ELSE (It's a user! 1:1 Private Message):
	//         - Find the user in the server by their nickname (target).
	//         - IF the user does not exist:
	//              SEND ERR_NOSUCHNICK (401) -> "No such nick/channel"
	//              RETURN
	//         - ELSE:
	//              Send the message directly to that user!
}