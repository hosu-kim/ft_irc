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
	std::string nick = user.getNickname().empty() ? "*" : user.getNickname();
	// 1. Parameter Validation:
	// - Check if the parameter count (getParamCount()) is less than 1.
	if (this->getParamCount() < 1) {
		// - If so, send ERR_NEEDMOREPARAMS (461) to the user and return.
		std::string errMsg = ":" + server.getServerName() + " 461 " + nick + " MODE :Not enough parameters";
		user.reply(errMsg);
		return;
	}
	
	// 2. Retrieve Target Channel:
	// - Get the target channel name from the first parameter (getParam(0)).
	std::string targetChan = this->getParam(0);
	// - Verify if it starts with '#' (indicating a channel).
	if (targetChan[0] != '#')
		return;
	// - Lookup the channel pointer in the server using server.getChannel().
	Channel* chan = server.getChannel(targetChan);
	// - If the channel does not exist, send ERR_NOSUCHCHANNEL (403) and return.
	if (chan == NULL) {
		std::string errMsg = ":" + server.getServerName() +" 403 " + nick + " " + targetChan + " :No such channel";
		user.reply(errMsg);
		return;
	}

	// 3. Handle Mode View Query (Only 1 Parameter provided, e.g. "MODE #channel"):

	// - If getParamCount() is exactly 1:
	if (this->getParamCount() == 1) {
	// - Retrieve the current modes and any mode arguments (like key or user limit) from the channel.
		std::string modes = "+";
		std::string modeParams = "";

		if (chan->hasMode('i')) {
			modes += "i";
		}
		if (chan->hasMode('t')) {
			modes += "t";
		}
		if (chan->hasMode('k')) {
			modes += "k";
			modeParams += " " + chan->getChannelKey();
		}
		if (chan->hasMode('l')) {
			modes += "l";
			std::stringstream ss;
			ss << chan->getMemberLimit();
			modeParams += " " + ss.str();
		}
		// - Send RPL_CHANNELMODEIS (324) to the user.
		std::string replyMsg = ":" + server.getServerName()+ " 324 " + nick + " " + targetChan + " " + modes + modeParams;
		user.reply(replyMsg);
		return;
	}

	// 4. Handle Mode Change (2 or more Parameters provided, e.g. "MODE #channel +k password"):
	// - Get the mode string from the second parameter (getParam(1)).
	std::string modeStr = this->getParam(1);
	// - The mode string must start with '+' or '-' (e.g., "+k", "-i").
	// - If it does not start with '+' or '-', it is invalid. Return to ignore.
	if (modeStr.empty() || (modeStr[0]) != '+' && modeStr[0] != '-') {
		return;
	}

	char op = modeStr[0];
	size_t paramIdx = 2; // Additional args start from param index 2
	
	// 5. Parse Modes and Parameters:
	for (size_t i = 1; i < modeStr.size(); ++i) {
		char mode_char = modeStr[i];
		bool needsArg = false;

		// - Check if the current mode character needs an argument
		if (mode_char == 'o' || (mode_char == 'k' && op == '+') ||
			(mode_char == 'l' && op == '+')) {
				needsArg = true;
			}

		std::string mode_arg = "";
		// - If needsArg is true:
		if (needsArg) {
			// - Check if paramIdx < getParamCount().
			if (paramIdx >= getParamCount()) {
				// - If not, send ERR_NEEDMOREPARAMS (461) to the user and return.
				std::string errMsg = ":" + server.getServerName() + " 461 " + nick + " MODE :Not enough parameters";
				user.reply(errMsg);
				return;
			}
			mode_arg = getParam(paramIdx++);
		}

		// - Call setModeon the channel
		int result = chan->setMode(mode_char, op, mode_arg, &user);

		// - Handle error codes or broadcast success

		if (result == 482) {
			std::string errMsg = ":" + server.getServerName() + " 482 " + nick + " " + targetChan + " :You're not channel operator";
			user.reply(errMsg);
			break;
		} else if (result == 401) {
			std::string errMsg = ":" + server.getServerName() + " 401 " + nick + " " + mode_arg + " :No such nick/channel";
			user.reply(errMsg);
		} else if (result == 472) {
			std::string errMsg = ":" + server.getServerName() + " 472 " + nick + " " + mode_char + " :is unknown mode character";
			user.reply(errMsg);
		} else if (result == 0) {
			std::string modeMsg = ":" + nick + "!" + user.getUserName() + "@" + user.getHostName() +" MODE " + targetChan + " " + op + mode_char;

			if (!mode_arg.empty()) {
				modeMsg += " " + mode_arg;
			}
			chan->broadcast(modeMsg, NULL);
		}
	}
}

