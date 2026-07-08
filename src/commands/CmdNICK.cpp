/**
 * Command: NICK
 * Description: Used to set or change the user's nickname.
 *              It is one of the essential commands for a user to register
 *              and be recognized by the IRC server.
 * 
 * Params: 0    1
 * Syntax: NICK <nickname>
 * 
 * Example: NICK a_cool_nick
 */

#include "CmdNICK.hpp"

void CmdNick::execute(User &user, Server &server) {
	std::string nick = user.getNickname().empty() ? "*" : user.getNickname();

	// Check if the parameter is missing
	if (getParamCount() != 1) {
		std::string err_msg = ":" + server.getServerName() + " 431 " + server.getUserName() + " :Nickname not given. Please provide us with your nickname.";
		user.reply(err_msg);
		return;
	}

	// Get the requested nickname
	std::string requested_nick = this->getParam(0);

	// Check invalid characters in the nickname
	// find_first_of(): returns the index of the first character that matches any character in the given string
	size_t pos = requested_nick.find_first_of(FORBIDDEN_CHARS);
	if (pos != std::string::npos) {
		std::string err_msg = ":" + server.getServerName() + " 432 " + server.getUserName() + " :invalid nickname";
		user.reply(err_msg);
		return;
	}

	// Check if the nickname is already taken by someone else in the server
	if (server.getUserByNick(getParam(0))) {
		std::string err_msg = ":" + server.getServerName() + " 433 " + nick + " " + requested_nick + " :nickname already exists in the server";
		user.reply(err_msg);
		return;
	}

	// Success: Apply the new nickname
	std::string old_nick = user.getNickname();
	std::string new_nick = getParam(0);
	user.setNickname(new_nick);
	user.setHasNick(true);

	std::map<std::string, Channel*> joined_channels = user.getJoinedChannels();

	std::map<std::string, Channel*>::iterator it;
	for (it = joined_channels.begin(); it != joined_channels.end(); ++it)
		it->second->updateUserNick(old_nick, new_nick);

	// Announce the nickname change
	std::string success_msg = ":" + old_nick + "!" + user.getUserName() + "@" + user.getHostName() + " NICK :" + user.getNickname();
	user.reply(success_msg);

	// Complete the registration
	if (!user.isRegistered() && user.getPassOK() == true && user.getHasNick() == true && user.getHasUser() == true) {
		user.setRegistered(true);
		std::string welcome_msg = ":" + server.getServerName() + " 001 " + user.getNickname() + " :Welcome to the ft_irc Network, " + user.getNickname() + "!" + user.getUserName() + "@" + user.getHostName();
		user.reply(welcome_msg);
	}
}