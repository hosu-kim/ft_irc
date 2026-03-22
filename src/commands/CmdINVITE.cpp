/* 
INVITE: Invites the given user to the given channel.

Syntax: INVITE <nickname> <channel>

example: INVITE friend123 #42prague
*/

#include "CmdINVITE.hpp"

void CmdInvite::execute(User &user, Server &server) {
	// (1) checks the number of parameters
	if (getParamCount() < 2) {
		// Sends ERR_NEEDMOREPARAMS (461)
		std::string msg = ":" + server.getName() + " 461" + user.getNickname()
						  + " INVITE :Not enough parameters";
		return;
	}
	
	std::string targetNick = getParam(0);
	std::string channelName = getParam(1);

	// (2) if the channel exists
	// (3) if invitor user exitst in the channel
	// (4) if `targetNick` exists in the server
	// (5) if `targetNick` is already in the channel
	// (6) if all conditions are true,
	//     (-) Send invite message to the given user
	//     (-) Send RPL_INVITING (341) message to the invitor.
}