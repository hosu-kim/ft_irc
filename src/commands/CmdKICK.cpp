/**
 * Command: KICK
 * Description: Used when a channel operator forcefully removes (=kicks)
 *              a user from a channel.
 * 
 * Params: 0    1         2      3
 * Syntax: KICK <channel> <user> [<reason>]
 * 
 * Example: KICK #42prague bad_user :Don't spam!
 */

#include "CmdKICK.hpp"

void CmdKick::execute(User &user, Server &server) {
	// "*" used as a temporary placeholder if the user hasn't set a nickname yet.
	// Prevent formatting errors in IRC numeric replies. (=> std::string msg = ...)
	std::string nick = user.getNickname().empty() ? "*" : user.getNickname();

	// Check parameter count
	if (getParamCount() < 2) {
		std::string err_msg = ":" + server.getServerName() + " 461 " + nick + " KICK :Not enough parameters";
		user.reply(err_msg);
		return;
	}

	// Extract parameters
	std::string channel_name = this->params_[0];
	std::string nick_being_kicked = this->params_[1];
	std::string reason = (this->params_.size() > 2) ? this->combineParamsToOneStr(2) : "No reason given"; // Default reason
	
	Channel* channel = server.getChannel(channel_name);

	// The channel doesn't exist in the server
	if (channel == NULL) {
		std::string err_msg = ":" + server.getServerName() + " 403 " + nick + " " + channel_name + " :No such channel";
		user.reply(err_msg);
		return;
	}


	// Check the privileges of kicker

	// User is NOT in the channel
	if (!channel->isUserInChannel(user.getNickname())) {
		std::string msg = ":" + server.getServerName() + " 442 " + nick + " " + channel_name + " :You're not on the channel";
		user.reply(msg);
		return;
	}
	// User is NOT an operator of the channel
	if (!channel->isUserOperator(user.getNickname())) {
		std::string err_msg = ":" + server.getServerName() + " 482 " + nick + " " + channel_name + " :You're not channel operator";
		user.reply(err_msg);
		return;
	}

	User* targetUser = channel->getUserByNick(nick_being_kicked);
	// Target is not in the channel
	if (targetUser == NULL) {
		std::string err_msg = ":" + server.getServerName() + " 441 " + nick + " " + nick_being_kicked + " " + channel_name + " :They aren't on that channel";
		user.reply(err_msg);
		return;
	}

	std::string kickMessage = ":" + nick + " KICK " + channel_name + " " + nick_being_kicked + " :" + reason;

	channel->broadcast(kickMessage, NULL); // Send message to all users in the channel
	channel->removeUser(targetUser); // Remove the target from the channel
}