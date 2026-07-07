/**
 * Command: JOIN
 * Description: Used when a user wants to join a specific channel.
 *              If the channel does not exist, it will be created automatically.
 * 
 * Params: 0.   1.        2.
 * Syntax: JOIN <channel> [<password>]
 * 
 * Examples:
 *    Case 1. Join a channel with NO PASSWORD: JOIN #42prague
 *    Case 2. Join a channel with PASSWORD: JOIN #42prague password123
 */

#include "CmdJOIN.hpp"

void CmdJoin::execute(User &user, Server &server) {
	// "*" used as a temporary placeholder if the user hasn't set a nickname yet.
	// Prevent formatting errors in IRC numeric replies. (=> std::string msg = ...)
	std::string nick = user.getNickname().empty() ? "*" : user.getNickname();

	// Check the number of parameters => one or tow
	if (getParamCount() < 1) {
		// Sends ERR_NEEDMOREPARAMS (461)
		std::string err_msg = ":" + server.getServerName() + " 461 " + nick + " JOIN :Not enough parameters";
		user.reply(err_msg);
		return;
	}
	
	std::string channel_name = this->getParam(0);
	std::string passowrd = (this->getParamCount() >= 2) ? this->getParam(1) : "";


	Channel* channel = server.getChannel(channel_name);

	// The channel doesn't exist in the server => Make one
	if (channel == NULL) {
		channel = server.setChannel(channel_name, &user);

	// The channel already exists in the server => Checks modes, +k and +l
	} else {
		// Check invite-only mode (+i)
		if (channel->hasMode('i') && !channel->isInvited(user.getNickname())) {
			std::string err_msg = ":" + server.getServerName() + " 473 " + nick + " " + channel_name + " :Cannot join channel (+i)";
			user.reply(err_msg);
			return; // 473 => ERR_INVITEONLYCHAN
		}
		// Check password (+k)
		if (channel->hasMode('k') && channel->getChannelKey() != passowrd) {
			std::string err_msg = ":" + server.getServerName() + " 475 " + nick + " " + channel_name + " :Cannot join channel (+k)";
			user.reply(err_msg);
			return; // 475 => ERR_BADCHANNELKEY
		}
		// Check num of users (+l)
		if (channel->hasMode('l') && channel->getMemberCount() >= channel->getMemberLimit()) {
			std::string err_msg = ":" + server.getServerName() + " 471 " + nick + " " + channel_name + " :Cannot join channel (+l)";
			user.reply(err_msg);
			return; // 471 => ERR_CHANNELISFULL
		}

		channel->addUser(&user, passowrd);

		if(channel->isInvited(user.getNickname()))
			channel->removeInvite(user.getNickname());
	}

	// Send a success message to all channel members
	// Format: "":nickname!user_name@host_name Join : #channel_name"
	std::string join_msg = ":" + nick + "!" + user.getUserName() + "@" + user.getHostName() + " JOIN :" + channel_name;
	channel->broadcast(join_msg, NULL);

	std::string topic = channel->getChannelTopic();

	// No topic is set in the channel
	if (topic.empty()) {
		std::string err_msg = ":" + server.getServerName() + " 331 " + nick + " " + channel_name + " :No topic is set";
		user.reply(err_msg);
	// Send the joined user the channel topic
	} else {
		std::string topic_msg = ":" + server.getServerName() + " 332 " + nick + " " + channel_name + " :" + channel->getChannelTopic();
		user.reply(topic_msg);
	}

	std::string user_list_msg = ":" + server.getServerName() + " 353 " + nick + " = " + channel_name + " :";

	// Iterate channel members to build nickname list
	const std::map<std::string, User*>& members = channel->getMembers();

	std::map<std::string, User*>::const_iterator it;
	for (it = members.begin(); it != members.end(); ++it) {
		if (channel->isUserOperator(it->first))
			user_list_msg += "@";

		user_list_msg += it->first + " ";
	}
	// Send RPL_NAMREPLY (353)
	user.reply(user_list_msg);

	std::string end_msg = ":" + server.getServerName() + " 366 " + nick + " " + channel_name + " :End of name list";
	user.reply(end_msg);
}
