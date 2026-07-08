/**
 * Command: TOPIC
 * Description: Used to change or view the topic of a channel.
 *              If a topic parameter is not provided, it returns the current topic of the channel.
 *              If provided, it sets the channel's topic to the new value.
 *              To clear a channel's topic, an empty topic string can be provided.
 * 
 * Params:       0         [1]
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
	
	// Parameter validation
	if (this->getParamCount() < 1) {
		std::string err_msg = ":" + server.getServerName() + " 461 " + nick + " TOPIC :Not enough parameters";
		user.reply(err_msg);
		return;
	}

	std::string channel_name = this->getParam(0);

	Channel *channel = server.getChannel(channel_name);
	
	// The channel doesn't exist
	if (channel == NULL) {
		std::string err_msg = ":" + server.getServerName() + " 403 " + nick + " " + channel_name + " :No such channel";
		user.reply(err_msg);
		return;
	}

	// The requesting user doesn't exist in the channel
	if (channel->isUserInChannel(user.getNickname()) == false) {
		std::string err_msg = ":" + server.getServerName() + " 442 " + nick + " " + channel_name + " :User is not in the channel";
		user.reply(err_msg);
		return;
	}

	
	// Only one parameter provided => View the topic
	if (this->getParamCount() == 1) {
		std::string channel_topic = channel->getChannelTopic();
		if (channel_topic == "") {
			std::string err_msg = ":" + server.getServerName() + " 331 " + nick + " " + channel_name + " :No topic is set";
			user.reply(err_msg);
			return;
		} else {
			std::string topic_msg = ":" + server.getServerName() + " 332 " + nick + " " + channel_name + " :" + channel_topic;
			user.reply(topic_msg);
		}
		return;
	}

	std::string new_topic = this->getParam(1);

	// Check permissions (Channel mode '+t') and validate the user privilege
	if (channel->hasMode('t') && channel->isUserOperator(user.getNickname()) == false) {
		std::string err_msg = ":" + server.getServerName() + " 482 " + nick + " " + channel_name + " :You're not channel operator";
		user.reply(err_msg);
		return;
	}

	// Update the topic
	channel->setChannelTopic(new_topic);

	// Broadcast the topic change => Notify all users in the channel that the topic has been changed.
	std::string broadcast_msg = ":" + nick + "!" + user.getUserName() + "@" + user.getHostName() + " TOPIC " + channel_name + " :" + new_topic;
	channel->broadcast(broadcast_msg, &user);
	user.reply(broadcast_msg);
}