/**
 * Command: MODE
 * Description: Used to change or query the active modes of a channel.
 *              Only channel operators cna modify modes (i, t, k, l, o).
 *              If no mode parameters are provided, it queries the active modes.
 * 
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

	// Parameter Validation
	if (this->getParamCount() < 1) {
		std::string err_msg = ":" + server.getServerName() + " 461 " + nick + " MODE :Not enough parameters";
		user.reply(err_msg);
		return;
	}

	std::string target_chan = this->getParam(0);
	// Verify if it starts with '#' (means the channel symbol).
	if (target_chan[0] != '#')
		return;

	Channel* chan = server.getChannel(target_chan);

	// the channel doesn't not exist in the server
	if (chan == NULL) {
		std::string err_msg = ":" + server.getServerName() +" 403 " + nick + " " + target_chan + " :No such channel";
		user.reply(err_msg);
		return;
	}

	// Handle Mode View Query (Only MODE and the channel name)

	if (this->getParamCount() == 1) {
		// Retrieve the current modes and any mode arguments (password, user limit) from the channel
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

		// - Send RPL_CHANNELMODEIS (324) to the user.
		std::string reply_msg = ":" + server.getServerName()+ " 324 " + nick + " " + target_chan + " " + modes + mode_params;
		user.reply(reply_msg);
		return;
	}

	// Handle Mode Change (Two or more parameters provided, e.g. "MODE #channel +k password"):

	// Get the mode string from the second parameter => getParam(1).
	std::string mode_param = this->getParam(1);
	// mode_str must start with '+' or '-' (e.g., "+k", "-i").
	// If not, Return
	if (mode_param.empty() || ((mode_param[0]) != '+' && mode_param[0] != '-'))
		return;

	char op = mode_param[0];
	size_t param_idx = 2;
	std::string applied_modes = "";
	std::string applied_params = "";
	char last_op = '\0';

	// Iterate mode_param (e.g., "+klo-i")
	for (size_t i = 1; i < mode_param.size(); ++i) {
		char mode_char = mode_param[i];

		// The current car is  a mode change operator ('+' or '-')
		if (mode_char == '+' || mode_char == '-') {
			op = mode_char;
			continue;
		}

		bool needs_arg = false;

		// The mode character needs an additional argument
		if ((mode_char == 'o') || (op == '+' && (mode_char == 'k' || mode_char == 'l')))
				needs_arg = true;

		std::string mode_arg = "";

		if (needs_arg) {
			if (param_idx >= this->getParamCount()) {
				std::string err_msg = ":" + server.getServerName() + " 461 " + nick + " MODE :Not enough parameters";
				user.reply(err_msg);
				return;
			}
			mode_arg = getParam(param_idx++);
		}

		int result = chan->setMode(mode_char, op, mode_arg, &user);

		// Handle error codes or broadcast success
		if (result == 482) {
			std::string errMsg = ":" + server.getServerName() + " 482 " + nick + " " + target_chan + " :You're not channel operator";
			user.reply(errMsg);
			break;
		} else if (result == 401) {
			std::string errMsg = ":" + server.getServerName() + " 401 " + nick + " " + mode_arg + " :No such nick/channel";
			user.reply(errMsg);
		} else if (result == 472) {
			std::string errMsg = ":" + server.getServerName() + " 472 " + nick + " " + mode_char + " :is unknown mode character";
			user.reply(errMsg);
		} else if (result == 0) {
			if (op != last_op) {
				applied_modes += op;
				last_op = op;
			}

			applied_modes += mode_char;

			if (!mode_arg.empty())
				applied_params += " " + mode_arg;
		}
	}

	if (!applied_modes.empty()) {
		std::string final_mode_msg = ":" + nick + "!" + user.getUserName() + "@" + user.getHostName() + " MODE " + target_chan + " " + applied_modes + applied_params;
		chan->broadcast(final_mode_msg, NULL);
	}
}
