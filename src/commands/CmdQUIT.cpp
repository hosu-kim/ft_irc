/**
 * Command: QUIT
 * Description: Disconnects the user from the server.
 *              An optional quit message can be provided, which is broadcasted to
 *              other user in the channels that the quitting user is a member of.
 * 
 * Params:      [0]
 * Syntax: QUIT [:<Quit message>]
 * 
 * Examples:
 *    Case 1. With message: QUIT :Bye everyone, leaving for dinner!
 *    Case 2. Without message: QUIT
 */

#include "CmdQUIT.hpp"

void CmdQuit::execute(User &user, Server &server) {
	std::string quit_msg = "";
	// The user being quit provided a message
	if (this->getParamCount() > 0)
		quit_msg = this->getParam(0);
	// the user being quit provided no message
	else
		quit_msg = "Client Quit";

	// Format the QUIT message to broadcast
	std::string nick = user.getNickname().empty() ? "*" : user.getNickname();
	std::string msg_in_format = ":" + nick + "!" + user.getUserName() + "@" + user.getHostName() + " QUIT :" + quit_msg;

	// Broadcast to the others and remove the user
	std::map<std::string, Channel*> joined_channels = user.getJoinedChannels();

	// Iterate channels the user has joined and leave from the channels one by one
	std::map<std::string, Channel*>::iterator it;
	for (it = joined_channels.begin(); it != joined_channels.end(); ++it) {
		Channel* channel = it->second;

		channel->broadcast(msg_in_format, &user);

		channel->removeUser(&user);

		if (channel->getUserCount() == 0) {
			server.removeChannel(channel->getChannelName());
		}
	}

	// Leave from the user
	server.removeUser(user);
}