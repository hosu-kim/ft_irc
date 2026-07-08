/**
 * Command: MODE
 * Description: Used to change or query the active modes of a channel.
 *              Only channel operators cna modify modes (i, t, k, l, o).
 *              If no mode parameters are provided, it queries the active modes.
 * 
 * Params: 0    1         [2]               [3]
 * Syntax: MODE <channel> [[+/-]<mode_char> [<mode_arg>]]
 * 
 * Examples:
 *    Case 1. Query active modes: MODE #42prague
 *    Case 2. Toggle invite-only: MODE #42prague +i
 *    Case 3. Set topic restriction: MODE #42prague +t
 *    Case 4. Set channel password: MODE #42prague +k password123
 *    Case 5. Limit member count: MODE #42prague +l 10
 *    Case 6. Toggle operator privileges: MODE #42prague +o user123
 */

#include "CmdMODE.hpp"

void CmdMode::execute(User &user, Server &server) {
	// "*" used as a temporary placeholder if the user hasn't set a nickname yet.
	// Prevent formatting errors in IRC numeric replies. (=> std::string msg = ...)
	std::string nick = user.getNickname().empty() ? "*" : user.getNickname();

	// No parameter is provided => Error
	if (this->getParamCount() < 1) {
		std::string err_msg = ":" + server.getServerName() + " 461 " + nick + " MODE :Not enough parameters";
		user.reply(err_msg);
		return;
	}

	std::string target_chan = this->getParam(0);
	// Verify if the first param starts with '#' (means the channel symbol).
	if (target_chan[0] != '#')
		return;

	Channel* chan = server.getChannel(target_chan);

	// the channel doesn't not exist in the server
	if (chan == NULL) {
		std::string err_msg = ":" + server.getServerName() +" 403 " + nick + " " + target_chan + " :No such channel";
		user.reply(err_msg);
		return;
	}

	/* Handle Mode View Query (Only MODE and the channel name) */

		// Retrieve the current modes and any mode arguments (password, user limit) from the channel
	if (this->getParamCount() == 1) {
		std::string modes = "+";
		std::string mode_params = "";

		if (chan->hasMode('i')) {
			modes += "i";
		}
		if (chan->hasMode('t')) {
			modes += "t";
		}
		if (chan->hasMode('k')) {
			modes += "k";
			mode_params += " " + chan->getChannelKey();
		}
		if (chan->hasMode('l')) {
			modes += "l";
			std::stringstream ss;
			ss << chan->getMemberLimit();
			mode_params += " " + ss.str();
		}

		// Send RPL_CHANNELMODEIS (324) to the user.
		std::string reply_msg = ":" + server.getServerName()+ " 324 " + nick + " " + target_chan + " " + modes + mode_params;
		user.reply(reply_msg);
		return;
	}

	/* Handle Mode Change Request (Two or more parameters provided, e.g. "MODE #channel +k password"): */

	std::string mode_flags = this->getParam(1);
	// mode_flags must start with '+'(add) or '-'(remove)
	// If not, Return
	if (mode_flags.empty() || ((mode_flags[0]) != '+' && mode_flags[0] != '-'))
		return;

	char sign = mode_flags[0];
	size_t arg_index = 2;
	std::string applied_modes = "";
	std::string applied_params = "";
	char last_sign = '\0';

	// Iterate mode_param to get single characters (e.g., "+klo-i")
	for (size_t i = 1; i < mode_flags.size(); ++i) {
		char mode_char = mode_flags[i];

		// The current char is a mode operator ('+' or '-')
		if (mode_char == '+' || mode_char == '-') {
			sign = mode_char;
			continue;
		}

		bool needs_additional_arg = false;

		// The mode character needs an additional argument: (+/-)o, +k, +l
		if ((mode_char == 'o') || (sign == '+' && (mode_char == 'k' || mode_char == 'l')))
				needs_additional_arg = true;

		std::string additional_arg = "";

		if (needs_additional_arg) {
			if (arg_index >= this->getParamCount()) {
				std::string err_msg = ":" + server.getServerName() + " 461 " + nick + " MODE :Not enough parameters";
				user.reply(err_msg);
				return;
			}
			additional_arg = getParam(arg_index++);
		}

		// Execute mode change request
		int result = chan->setMode(mode_char, sign, additional_arg, &user);

		// Handle error codes or broadcast success
		if (result == 482) {
			std::string err_msg = ":" + server.getServerName() + " 482 " + nick + " " + target_chan + " :You're not channel operator";
			user.reply(err_msg);
			break;
		} else if (result == 401) {
			std::string err_msg = ":" + server.getServerName() + " 401 " + nick + " " + additional_arg + " :No such nick/channel";
			user.reply(err_msg);
		} else if (result == 472) {
			std::string err_msg = ":" + server.getServerName() + " 472 " + nick + " " + mode_char + " :is unknown mode character";
			user.reply(err_msg);
		} else if (result == 0) {
			// Tracks the last applied sign (+ or -) to avoid redundant prefixes in the final mode string.
			if (sign != last_sign) {
				applied_modes += sign;
				last_sign = sign;
			}

			applied_modes += mode_char;

			if (!additional_arg.empty())
				applied_params += " " + additional_arg;
		}
	}

	if (!applied_modes.empty()) {
		std::string final_mode_msg = ":" + nick + "!" + user.getUserName() + "@" + user.getHostName() + " MODE " + target_chan + " " + applied_modes + applied_params;
		chan->broadcast(final_mode_msg, NULL);
	}
}
