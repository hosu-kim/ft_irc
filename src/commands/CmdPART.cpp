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

std::string CmdPart::join_params_from_index(std::size_t start) const {
	std::string out;
	for (std::size_t i = start; i < this->getParamCount(); ++i) {
		if (!out.empty()) out += ' ';
		out += this->getParam(i);
	}
	if (!out.empty() && out.front() == ':') out.erase(out.begin());
	return out;
}

/*
:<server> 461 <nick> PART :Not enough parameters\r\n
:<server> 403 <nick> <channel> :No such channel\r\n
:<server> 442 <nick> <channel> :You're not on that channel\r\n
*/

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
	// - channels_param is first parameter, could be comma-separated list of channels
	// - optional message might be provided as the second param or as a single param starting with ':'.
	std::string channels_param = this->getParam(0);
	std::string part_reason = ""; // default: empty
	if (this->getParamCount() > 1) {
		// join remaining params into message, and strip leading ':' if present
		// e.g., params[1] might be ":Goodbye" or multiple tokens
		part_reason = join_params_from_index(1); // helper: combine with spaces
		if (part_reason starts with ':') remove leading ':';
	}

	// 3) Split channels by comma and iterate
	vector<string> channel_names = split(channels_param, ',');
	for each channel_name in channel_names {
		// trim whitespace around channel_name if needed

		// 4) Validate channel existence
		Channel *chan = server.findChannel(channel_name);
		if (chan == nullptr) {
			// send ERR_NOSUCHCHANNEL (no such channel) to user
			// continue to next channel (do not abort whole command)
			continue;
		}

		// 5) Check whether user is on the channel
		if (!chan->hasMember(user)) {
			// send ERR_NOTONCHANNEL to user
			continue;
		}

		// 6) Remove user from channel
		// - Remove user from channel's member list
		// - If the user was an operator, adjust operator list as needed
		// - If removal changes channel state (e.g., empty), optionally destroy the channel
		chan->removeMember(user);

		// 7) Notify the remaining channel members
		// - Broadcast a PART message to channel members:
		//   Format example (conceptual): ":" + user.getPrefix() + " PART " + channel_name + (part_message.empty() ? "" : " :" + part_message)
		// - Use server/channel broadcast helper to send message to all other members
		chan->broadcastPart(user, channel_name, part_reason);

		// 8) Post-removal housekeeping
		// - If channel has no members now, clean it up (delete or mark inactive) depending on server design
		// - If there are channel-specific modes or state to update, handle them
	}

	// 9) End: no global reply needed if every channel was processed;
	// errors were sent per-channel when necessary.
}