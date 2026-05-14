/**
 * Command: JOIN
 * Description: Used when a user wants to join a specific channel.
 *              If the channel does not exist, it will be created automatically.
 * 
 * Syntax: JOIN <channel> [<password>]
 * 
 * Examples:
 *    Case 1. Join a channel with NO PASSWORD: JOIN #42prague
 *    Case 2. Join a channel with PASSWORD: JOIN #42prague password123
 */

#include "CmdJOIN.hpp"


void CmdJoin::execute(User &user, Server &server) {
	// "*" used as a temporary placeholder if the user hasn't set a nickname yet.
	// This prevents formatting errors in IRC numeric replies. (=> std::string msg = ...)
	std::string nick = user.getNickname().empty() ? "*" : user.getNickname();

	/*
	 * 1. Check the number of parameters
	 */
	if (getParamCount() < 1) {
		// Sends ERR_NEEDMOREPARAMS (461)
		std::string errMsg = ":" + server.getServerName() + " 461 " + nick
						  + " JOIN :Not enough parameters";
		user.reply(errMsg);
		return;
	}
	
	std::string channelName = getParam(0);
	std::string key = (getParamCount() >= 2) ? getParam(1) : "";

	/* 
	 * Find a channel with the given channel userName in the server
	 */
	Channel* channel = server.getChannel(channelName);

	// Case 1: the channel doesn't exist in the server => make one
	if (channel == NULL) {
		
		// the first user in the channel becomes an operator,
		// it should be implemented in createChannel for now idk
		channel = server.setChannel(channelName, &user);

	// Case 2: the channel already exists in the server: checks modes, +k and +l
	} else {
		// A. Invite-only Mode Check (Mode +i)
		if (channel->hasMode('i') && !channel->isInvited(user.getNickname())) {
			user.reply(":" + server.getServerName() + " 473 " + nick + " " + channelName + " :Cannot join channel (+i)");
			return; // 473 => ERR_INVITEONLYCHAN
		}
		// B. Password Check (Mode +k)
		if (channel->hasMode('k') && channel->getChannelKey() != key) {
			user.reply(":" + server.getServerName() + " 475 " + nick + " " + channelName + " :Cannot join channel (+k)");
			return; // 475 => ERR_BADCHANNELKEY
		}
		// C. Memeber Limit Check (Mode +l)
		if (channel->hasMode('l') && channel->getMemberCount() >= channel->getMemberLimit()) {
			user.reply(":" + server.getServerName() + " 471 " + nick + " " + channelName + " :Cannot join channel (+l)");
			return; // 471 => ERR_CHANNELISFULL
		}
		// D. Add the user
		channel->addUser(&user, key);

		if(channel->isInvited(user.getNickname()))
			channel->removeInvite(user.getNickname());
	}
	// (3) Sends a success message to every channel member
	// Format: "":nickName!userName@hostName Join : #channelName"
	std::string joinMsg = ":" + nick + "!" + user.getUserName() + "@" + user.getHostName() + " JOIN :" + channelName + "\r\n";
	channel->broadcast(joinMsg, NULL);
}
