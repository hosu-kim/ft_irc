/**
 * Command: QUIT
 * Description: Disconnects the user from the server.
 *              An optional quit message can be provided, which is broadcasted to
 *              other user in the channels that the quitting user is a member of.
 * 
 * Syntax: QUIT [:<Quit message>]
 * 
 * Examples:
 *    Case 1. With message: QUIT :Bye everyone, leaving for dinner!
 *    Case 2. Without message: QUIT
 */

#include "CmdQUIT.hpp"

void CmdQuit::execute(User &user, Server &server) {
	/*
	 * 1. Determine the quit msg (reason)
	 */
	// If the user provided a message (getParamCount() > 0):
	//      quitMsg = getParam(0)
	// Else:
	//      quitMsg = "Client Quit" (or any default message)

	/*
	 * 2. Format the QUIT message to broadcast
	 */
	// formattedMsg = ":" + user.getNickname() + "!" + user.getUserName() + "@" + user.getHostName() + " QUIT :" + quitMsg;

	/*
	 * 3. Broadcast to others & Remove the user
	 */
	// OPTION A: If server.removeUser() handles broadcasting to channels automatically:
	//      server.method_to_remove_user(user, formattedMsg); // Pass the msg so server can broadcast it
	
	// OPTION B: If you need to manually broadcast first:
	//      - Find all channels this user is in.
	//      - channel->broadcast(formattedMsg, &user);
	//      - server.method_to_remove_user(user); 
}