/**
 * Command: QUIT
 * Description: Disconnects the user from the server.
 *              An optional quit message can be provided, which is broadcasted to
 *              other user in the channels that the quitting user is a member of.
 * 
 * Syntax: QUIT [:<Quit message>]
 * 
 * Examples:
 *    Case 1. With message: QUIT :Bye everyone, leaving for dinner!
 *    Case 2. Without message: QUIT
 */

#include "CmdQUIT.hpp"

void CmdQuit::execute(User &user, Server &server) {
	/*
	 * 1. Determine the quit msg (reason)
	 */
	std::string quitMsg = "";
	// 1a. If the user provided a message
	if (this->getParamCount() > 0)
		quitMsg = this->getParam(0);
	// 1b. user provided no message
	else
		quitMsg = "Client Quit";

	/*
	 * 2. Format the QUIT message to broadcast
	 */
	std::string nick = user.getNickname().empty() ? "*" : user.getNickname();
	std::string formattedMsg = ":" + nick + "!" + user.getUserName() + "@" + user.getHostName() + " QUIT :" + quitMsg;

	/*
	 * 3. Broadcast to others & Remove the user
	 */
	std::map<std::string, Channel*> joinedChannels = user.getJoinedChannels();
	// - Find all channels this user is in.
	// - channel->broadcast(formattedMsg, &user);
	for (std::map<std::string, Channel*>::iterator it = joinedChannels.begin(); it != joinedChannels.end(); ++it) {
		Channel* channel = it->second;

		channel->broadcast(formattedMsg, &user);

		channel->removeUser(&user);

		if (channel->getUserCount() == 0) {
			server.removeChannel(channel->getChannelName());
		}
	}
	server.removeUser(user);
	//      - server.method_to_remove_user(user);
}