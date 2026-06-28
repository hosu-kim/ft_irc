/**
 * Command: NICK
 * Description: Used to set or change the user's nickname.
 *              It is one of the essential commands for a user to register
 *              and be recognized by the IRC server.
 * 
 * Syntax: NICK <nickname>
 * 
 * Example: NICK a_cool_nick
 */

#include "CmdNICK.hpp"

void CmdNick::execute(User &user, Server &server)
{
	std::string nick = user.getNickname().empty() ? "*" : user.getNickname();

	/*
	 * 1. Check if the nickname parameter is missing
	 */
	if (getParamCount() != 1)
	{
		std::string errMsg = ":" + server.getServerName() + " 431 " + server.getUserName() + " :Nickname not given. Please provide us with your nickname.";
		user.reply(errMsg);
		return;
	}

	/*
	 * 2. Get the requested nickname.
	 */
	std::string requested_nick = getParam(0);
	
	/*
	 * 3. Check for invalid characters in the nickname
	 */
	// find_first_of: - returns the index of the first character that matches any character in the given string
	//                - C++98 standard library function(<string>)
	size_t pos = requested_nick.find_first_of(FORBIDDEN_CHARS);
	if (pos != std::string::npos) {
		std::string errMsg = ":" + server.getServerName() + " 432 " + server.getUserName() + " :invalid nickname";
		user.reply(errMsg);
		return;
	}

	/*
	 * 4. Check if the nickname is already taken by someone else in the server
	 */
	// // Using _params[i] directly could cause a segfault if the requested index doesn't exist. => Use getParam()
	if (server.getUserByNick(getParam(0))) {
		std::string errMsg = ":" + server.getServerName() + " 433 " + nick + " " + requested_nick + " :nickname already exists in the server";
		user.reply(errMsg);
		return;
	}

	/*
	 * 5. Success! Apply the new nickname
	 */
	std::string oldNick = user.getNickname();
	std::string newNick = getParam(0);
	user.setNickname(newNick);
	user.setHasNick(true);

	std::map<std::string, Channel*> joined = user.getJoinedChannels();

	for (std::map<std::string, Channel*>::iterator it = joined.begin(); it != joined.end(); ++it) {
		it->second->updateUserNick(oldNick, newNick);
	}

	/*
	 * 6. Announce the NICK change
	 */
	std::string success_msg = ":" + oldNick + "!" + user.getUserName() + "@" + user.getHostName() + " NICK :" + user.getNickname();
	user.reply(success_msg);

	/**
	 * 7. Check if registration is completed after setting the nickname
	 */
	if (!user.getRegistered() && user.getPassOK() == true && user.getHasNick() == true && user.getHasUser() == true) {
		user.setRegistered(true);
		std::string welcomeMsg = ":" + server.getServerName() + " 001 " + user.getNickname() + " :Welcome to the ft_irc Network, " + user.getNickname() + "!" + user.getUserName() + "@" + user.getHostName();
		user.reply(welcomeMsg);
	}
}