/**
 * Command: PRIVMSG
 * Description: Sends a message to a specific user or a channel.
 *              Despite the name "PRIV", it is used for both 1:1 private messages
 *              and public channel messages.
 * 
 * Params:         0         1
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

	// Parameter validation: No param given
	if (this->getParamCount() == 0) {
		std::string err_msg = ":" + server.getServerName() + " 411 " + nick + " :No recipient given";
		user.reply(err_msg);
		return;
	}

	// Only 1 parameter given (= has target, but no message):
	if (this->getParamCount() == 1) {
		std::string err_msg = ":" + server.getServerName() + " 412 " + nick + " :No text to send";
		user.reply(err_msg);
		return;
	}

	// Extract Target and the message
	std::string target = this->getParam(0);
	std::string chat_msg = this->combineParamsToOneStr(1);

	/* Route the message: is it sent to a channel or a user? */

	// If target starts with '#' (= target is a CHANNEL => send chat_msg to channel)
	if (target[0] == '#') { 
		Channel* target_channel = server.getChannel(target);

		// If the channel does not exist:
		if (target_channel == NULL) {
			std::string err_msg = ":" + server.getServerName() + " 403 " + nick + " " + target + " :Channel does not exist";
			user.reply(err_msg);
			return;
		}

		// If the sender is not in that channel:
		if (target_channel->getUserByNick(user.getNickname()) == NULL) {
			std::string err_msg = ":" + server.getServerName() + " 404 " + nick + " " + target + " :User is not in channel";
			user.reply(err_msg);
			return;
		}
		// Send chat_msg to the channel
		else {
			std::string chat_msg_in_format = ":" + user.getNickname() + "!"
										 + user.getUserName() + "@"
										 + user.getHostName() + " PRIVMSG "
										 + target + " :" + chat_msg;
			target_channel->broadcast(chat_msg_in_format, &user);
		}
	} 
	// If target is a user => send chat_msg to the user.
	else {
		// Find the user in the server
		User* target_user = server.getUserByNick(target);

		// The user does not exist
		if (target_user == NULL) {
			std::string err_msg = ":" + server.getServerName() + " 401 " + nick + " :No such nick";
			user.reply(err_msg);
			return;
		}
		// Aend chat_msg to the user
		else {
			std::string chat_msg_in_format = ":" + user.getNickname() + "!"
										 + user.getUserName() + "@"
										 + user.getHostName() + " PRIVMSG "
										 + target_user->getNickname() + " :" + chat_msg;
			target_user->reply(chat_msg_in_format);
		}
	}
}