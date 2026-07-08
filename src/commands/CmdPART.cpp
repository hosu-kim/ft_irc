/**
 * Command: PART
 * Description: Used to leave one or more channels that the user has joined.
 *              If a message is provided, it'll be sent to the channel members.
 *              This command allows a user to depart from channels.
 * 
 * Params: 0    1                     [2]
 * Syntax: PART <channel>[,<channel>] [<message>]
 * 
 * Examples:
 *    PART #chatroom
 *    PART #chatroom,#other :Goodbye everyone!
 * 
 * Note: If the user is not a member of a specified channel, the server should
 *       respond with an appropriate error (ERR_NOTONCHANNEL).
 */

#include "CmdPART.hpp"

void CmdPart::execute(User &user, Server &server) {
	// "*" used as a temporary placeholder if the user hasn't set a nickname yet.
	// This prevents formatting errors in IRC numeric replies. (=> std::string msg = ...)
	std::string nick = user.getNickname().empty() ? "*" : user.getNickname();

	// If no params provided => send ERR_NEEDMOREPARAMS (461) and return
	if (this->getParamCount() == 0) {
		std::string err_msg = ":" + server.getServerName() + " 461 " + user.getNickname() + " PART :Not enough parameters";
		user.reply(err_msg);
		return;
	}

	// Parse channels and optional part message
	// NOTE 1. Channels_param is first parameter and it could be comma-separated list of channels => PART #ch1,ch2...
	// NOTE 2. Optional message might be provided as the second param or as a single param starting with ':' => PART #ch :bye bye
	std::string channels_param = this->getParam(0);
	std::string part_reason = ""; // default: empty
	if (this->getParamCount() > 1)
		// join remaining params into message, and strip leading ':' if present
		part_reason = combineParamsToOneStr(1);

	// Split channels by comma and iterate
	std::vector<std::string> channel_names = splitStr(',');
	for (size_t i = 0; i < channel_names.size(); ++i) {
		std::string &channel_name = channel_names[i];

		// Validate channel existence
		Channel *chan = server.getChannel(channel_name);
		if (chan == NULL) {
			std::string err_msg = ":" + server.getServerName() + " 403 " + nick + " " + channel_name + " :No such channel";
			user.reply(err_msg);
			continue;
		}

		// Check if user is on the channel
		if (!chan->isUserInChannel(user.getNickname())) {
			std::string err_msg = ":" + server.getServerName() + " 442 " + nick + " " + channel_name + " :You're not on that channel";
			user.reply(err_msg);
			continue;
		}

		// Notify the channel members
		std::string part_msg = ":" + nick + "!" + user.getUserName() + "@" + user.getHostName() + " PART " + channel_name;
		if (!part_reason.empty())
			part_msg += " :" + part_reason;
		chan->broadcast(part_msg, NULL);

		// Remove the user from the channel
		chan->removeUser(&user);
		user.leaveChannel(chan);

		// 8. Clean up the channel if it has no members left
		if (chan->getMemberCount() == 0) {
			server.removeChannel(channel_name);
		}
	}
}