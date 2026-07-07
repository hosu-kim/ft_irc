/**
 * Command: INVITE
 * Description: Invites the given user to the given channel.
 *              It is especially usuful for allowing users to join
 *              invite-only channels (mode +i).
 * 
 * Params: 0.     1.         2.
 * Syntax: INVITE <nickname> <channel>
 * 
 * Example: INVITE friend123 #42prague
 */

#include "CmdINVITE.hpp"


void CmdInvite::execute(User &user, Server &server) {
	// The number of parameters is less than 2 (Need two params)
	if (this->getParamCount() < 2) {
		// Send ERR_NEEDMOREPARAMS (461)
		std::string err_msg = ":" + server.getServerName() + " 461 " + user.getNickname() + " INVITE :Not enough parameters";
		user.reply(err_msg);
		return;
	}
	
	std::string nick_being_invited = this->getParam(0);
	std::string channel_name = this->getParam(1);
	Channel* channel = server.getChannel(channel_name);

	User* target_user = server.getUserByNick(nick_being_invited);
	// Target_nick doesn't exitst in the server
	if (target_user == NULL) {
		std::string err_msg = ":" + server.getServerName() + " 401 " + user.getNickname() + " " + nick_being_invited + " :No such nick/channel";
		user.reply(err_msg);
		return;
	}

	// The channel doesn't exist
	if (channel == NULL) {
		// Format: ":server_name 403 target_nick channel_name: No such Channel";
		// 403: ERR_NOSUCHCHANNEL
		std::string err_msg = ":" + server.getServerName() + " 403 " + user.getNickname() + " " + channel_name + " :No such Channel";
		user.reply(err_msg);
		return;
	}

	// Invitor doesn't exist in the channel
	if (!channel->isUserInChannel(user.getNickname())) {
		std::string err_msg = ":" + server.getServerName() + " 442 " + user.getNickname() + " :You're not on that channel";
		user.reply(err_msg);
		return;
	}

	// target_nick already exists in the channel
	if (channel->isUserInChannel(nick_being_invited)) {
		std::string err_msg = ":" + server.getServerName() + " 443 " + user.getNickname() + " :is already on channel";
		user.reply(err_msg);
		return;
	}

	// Success: send a message and add target_Nick to invited_Users_`
	channel->addInvite(nick_being_invited);
	// Send invite message to the target user
	std::string inviteMsg = ":" + user.getNickname() + "!" + user.getUserName() + "@" + user.getHostName() + " INVITE " + nick_being_invited + " :" + channel_name;
	target_user->reply(inviteMsg);
	// Send RPL_INVITING (341) => success message to the invitor.
	std::string rplMsg = ":" + server.getServerName() + " 341 " + user.getNickname() + " " + nick_being_invited + " " + channel_name;
	user.reply(rplMsg);
}