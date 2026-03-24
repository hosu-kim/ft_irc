/* JOIN: used when user joins a channel

   Syntax: JOIN <channel> [<password>]

   Example: Case 1. Join a channel with NO PASSWORD: JOIN #42prague
            Case 2. Join a channel with PASSWORD: JOIN #42prague password123
*/

#include "CmdJOIN.hpp"

void CmdJoin::execute(User &user, Server &server) {
	// (1) checks the number of parameters
	if (getParamCount() < 1) {
		// Sends ERR_NEEDMOREPARAMS (461)
		std::string msg = ":" + server.getUserName() + " 461" + user.getNickname()
						  + " JOIN :Not enough parameters";
		return;
	}
	
	std::string channelName = getParam(0);
	std::string key = (getParamCount() >= 2) ? getParam(1) : "";

	// (2) Find a channel with the given channel userName in the server
	Channel* channel = server.findChannel(channelName);

	// Case 1: the channel doesn't exist in the server => make one
	if (channel == NULL) {
		
		// the first user in the channel becomes an operator,
		// it should be implemented in createChannel for now idk
		channel = server.createChannel(channelName, &user);

	// Case 2: the channel already exists in the server: checks modes, +k and +l
	} else {
		// A. Invite-only Mode Check (Mode +i)
		if (channel->hasMode('i') && !channel->isInvited(user.getNickname())) {
			user.reply(":" + server.getServerName() + " 473 " + user.getNickname() + " " + channelName + " :Cannot join channel (+i)");
			return; // 473 => ERR_INVITEONLYCHAN
		}
		// B. Password Check (Mode +k)
		if (channel->hasMode('k') && channel->getChannelKey() != key) {
			user.reply(":" + server.getUserName() + " 475 " + user.getNickname() + " " + channelName + " :Cannot join channel (+k)");
			return; // 475 => ERR_BADCHANNELKEY
		}
		// C. Memeber Limit Check (Mode +l)
		if (channel->hasMode('l') && channel->getMemberCount() >= channel->getMemberLimit()) {
			user.reply(":" + server.getUserName() + " 471 " + user.getNickname() + " " + channelName + " :Cannot join channel (+l)");
			return; // 471 => ERR_CHANNELISFULL
		}
		// D. Add the user
		channel->addUser(&user, key);
	}
	// (3) Sends a success message to every channel member
	// Format: "":nickName!userName@hostName Join : #channelName"
	std::string joinMsg = ":" + user.getNickname() + "!" + user.getUserName() + "@" + user.getHostName() + " JOIN :" + channelName;
	channel->broadcast(joinMsg, NULL);
}
