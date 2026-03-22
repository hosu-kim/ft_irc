/* 
JOIN: used when user joins a channel
Syntax: JOIN <channel> [<password>]
Example: Case 1. Join a channel with NO PASSWORD: JOIN #42prague
         Case 2. Join a channel with PASSWORD: JOIN #42prague password123
*/

#include "CmdJOIN.hpp"

void CmdJoin::execute(User &user, Server &server) {
	// (1) checks the number of parameters
	if (getParamCount() < 1) {
		// Sends ERR_NEEDMOREPARAMS (461)
		std::string msg = ":" + server.getName() + " 461" + user.getNickname()
						  + " JOIN :Not enough parameters";
		return;
	}
	
	std::string channelName = getParam(0);
	std::string key = (getParamCount() >= 2) ? getParam(1) : "";


}