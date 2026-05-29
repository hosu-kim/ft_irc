/**
 * Command: PART
 * Description: Used to leave one or more channels that the user has joined.
 *              If a message is provided, it'll be sent to the channel members.
 *              This command allows a user to depart from channels.
 * 
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

	/*
	 * 1. Parameter validation: If no params provided => send ERR_NEEDMOREPARAMS (461) and return
	 */
	if (this->getParamCount() == 0) {
		std::string errMsg = ":" + server.getServerName() + " 461 " + user.getNickname() + " PART :Not enough parameters";
		user.reply(errMsg);
		return;
	}

	/*
	 * 2. Parse channels and optional part message
	 */
	// NOTE 1. channels_param is first parameter, could be comma-separated list of channels => PART #ch1,ch2...
	// NOTE 2. optional message might be provided as the second param or as a single param starting with ':' => PART #ch :bye bye
	std::string channels_param = this->getParam(0);
	std::string partReason = ""; // default: empty
	if (this->getParamCount() > 1) {
		// join remaining params into message, and strip leading ':' if present
		// e.g., params[1] might be ":Goodbye" (1 token) or multiple tokens => Goobye everyone (2 tokens)
		partReason = combine_params_with_spaces(1);
	}

	/*
	 * 3. Split channels by comma and iterate
	 */
	std::vector<std::string> channelNames = split_str(',');
	for (size_t i = 0; i < channelNames.size(); ++i) {
		std::string &channelName = channelNames[i];

		/*
		 * 4. Validate channel existence
		 */
		Channel *chan = server.getChannel(channelName);
		if (chan == NULL) {
			std::string errMsg = ":" + server.getServerName() + " 403 " + nick + " " + channelName + " :No such channel";
			user.reply(errMsg);
			continue;
		}

		/* 
		 * 5. Check whether user is on the channel
		 */
		if (!chan->isUserInChannel(user.getNickname())) {
			std::string errMsg = ":" + server.getServerName() + " 442 " + nick + " " + channelName + " :You're not on that channel";
			user.reply(errMsg);
			continue;
		}

		/* 
		 * 6. Notify the channel members
		 */
		std::string partMsg = ":" + nick + "!" + user.getUserName() + "@" + user.getHostName() + " PART " + channelName;
		if (!partReason.empty())
			partMsg += " :" + partReason;
		chan->broadcast(partMsg, NULL);

		/* 
		 * 7. Remove the user from the channel
		 */
		chan->removeUser(&user);
		user.removeChannel(chan);

		/* 
		 * 8. Clean up the channel if it has no members left
		 */
		if (chan->getMemberCount() == 0) {
			server.removeChannel(channelName);
		}
	}
}