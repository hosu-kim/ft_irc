#include "CmdKICK.hpp"

void CmdKick::execute(User &user, Server &server) {
	/* 
	 * 1. Check parameter count
	 * Syntax: KICK <channel> <target_user> [<reason>]
	 */
	if (getParamCount() < 2) {
		std::string msg = ":" + server.getServerName() + " 461 " + user.getNickname() + " KICK :Not enough parameters";
		user.reply(msg);
		return;
	}

	/* 
	 * 2. Extract parameters for readability
	 */
	std::string channelName = this->_params[0];
	std::string targetNick = this->_params[1];
	std::string reason = (this->_params.size() > 2) ? this->_params[2] : "No reason given"; // Default reason
	/* 
	 * 3. Check if the channel exists in the Server
	 */
	Channel* channel = server.findChannel(channelName);
	if (channel == NULL) {
		std::string msg = ":" + server.getServerName() + " 403 " + user.getNickname() + " " + channelName + " :No such channel";
		user.reply(msg);
		return;
	}

	/* 
	 * 4. Check the privileges of the command sender (user)
	 */
	// IF 'user' is NOT in 'channel':
	if (!channel->isUserInChannel(user.getNickname())) {
		std::string msg = ":" + server.getServerName() + " 442 " + user.getNickname() + " " + channelName + " :You're not on that channel";
		user.reply(msg);
		return;
	}
	// IF 'user' is NOT an operator of 'channel':
	if (!channel->isUserOperator(user.getNickname())) {
		std::string msg = ":" + server.getServerName() + " 482 " + user.getNickname() + " " + channelName + " :You're not channel operator";
		user.reply(msg);
		return;
	}

	/* 
	 * 5. Check if the target user is in the channel
	 */
	User* targetUser = channel->findUserByNick(targetNick);
	if (targetUser == NULL) {
		std::string msg = ":" + server.getServerName() + " 441 " + user.getNickname() + " " + targetNick + " " + channelName + " :They aren't on that channel";
		user.reply(msg);
		return;
	}

	/* 
	 * 6. Execute KICK! (Broadcast FIRST, then Remove)
	 */
	std::string kickMessage = ":" + user.getNickname() + " KICK " + channelName + " " + targetNick + " :" + reason;

	channel->broadcast(kickMessage, NULL); // Sends message to everyone including the target and sender
	channel->removeUser(targetUser); // Removes the target user from the channel.
}