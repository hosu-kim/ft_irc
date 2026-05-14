/**
 * Command: TOPIC
 * Description: Used to change or view the topic of a channel.
 *              If a topic parameter is not provided, it returns the current topic of the channel.
 *              If provided, it sets the channel's topic to the new value.
 *              To clear a channel's topic, an empty topic string can be provided.
 * 
 * Syntax: TOPIC <channel> [<topic>]
 * 
 * Examples:
 *    Case 1. View topic: TOPIC #channel
 *    Case 2. Set topic: TOPIC #channel :This is the new channel topic!
 *    Case 3. Clear topic: TOPIC #channel :
 */

#include "CmdTOPIC.hpp"

void CmdTopic::execute(User &user, Server &server) {
	// "*" used as a temporary placeholder if the user hasn't set a nickname yet.
	// This prevents formatting errors in IRC numeric replies. (=> std::string msg = ...)
	std::string nick = user.getNickname().empty() ? "*" : user.getNickname();
	/* 
	 * 1. Check if there are enough parameters
	 */
	// If there are no parameters (not even a channel name), return an error.
	if (this->getParamCount() < 1) {
		std::string errMsg = ":" + server.getServerName() + " 461 " + nick + " TOPIC :Not enough parameters";
		user.reply(errMsg);
		return;
	}

	std::string channelName = this->getParam(0);

	/* 
	 * 2. Check if the channel exists
	 */
	// Find the channel object in the server using the channel name.
	Channel *channel = server.getChannel(channelName);
	if (channel == NULL) {
		std::string errMsg = ":" + server.getServerName() + " 403 " + nick + " " + channelName + " :No such channel";
		user.reply(errMsg);
		return;
	}

	/* 
	 * 3. Check if the user is a member of the channel
	 */
	// A user must be on the channel to view or change its topic.
	if (channel->isUserInChannel(user.getNickname()) == false) {
		std::string errMsg = ":" + server.getServerName() + " 442 " + nick + " " + channelName + " :User is not in the channel";
		user.reply(errMsg);
		return;
	}

	/*
	 * 4. CASE A: View the topic (Only 1 parameter provided)
	 */
	if (this->getParamCount() == 1) {
		std::string channelTopic = channel->getChannelTopic();
		if (channelTopic == "") {
			std::string errMsg = ":" + server.getServerName() + " 331 " + nick + " " + channelName + " :No topic is set";
			user.reply(errMsg);
			return;
		} else {
			std::string sendTopic = ":" + server.getServerName() + " 332 " + nick + " " + channelName + " :" + channelTopic;
			user.reply(sendTopic);
		}
		return;
	}

	/*
	 * 5. CASE B: Set or clear the topic (2 or more parameters provided)
	 */
	std::string newTopic = this->getParam(1);

	// 5a. Check permissions (Channel mode '+t')
	// If the channel has '+t' mode enabled (topic settable by channel operators only),
	// and the user is NOT a channel operator, return an error.
	if (channel->hasMode('t') && channel->isUserOperator(user.getNickname()) == false) {
		std::string errMsg = ":" + server.getServerName() + " 482 " + nick + " " + channelName + " :You're not channel operator";
		user.reply(errMsg);
		return;
	}

	// 5b. Update the topic
	channel->setChannelTopic(newTopic);

	// 5c. Broadcast the topic change => Notify all users in the channel that the topic has been changed.
	std::string broadcastMsg = ":" + nick + "!" + user.getUserName() + "@" + user.getHostName() + " TOPIC " + channelName + " :" + newTopic;
	channel->broadcast(broadcastMsg, &user);
	user.reply(broadcastMsg);
}