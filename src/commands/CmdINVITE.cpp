/* INVITE: Invites the given user to the given channel.

   Syntax: INVITE <nickname> <channel>

   example: INVITE friend123 #42prague
*/

#include "CmdInvite.hpp"

/* ORTHODOX CANONICAL FORM */
// vv default constructor
CmdInvite::CmdInvite() : ACmd("", std::vector<std::string>()) {}
// vv parameterized constructor
CmdInvite::CmdInvite(std::string cmd, std::vector<std::string> params)
	: ACmd(cmd, params) {}
// vv copy constructor
CmdInvite::CmdInvite(const CmdInvite& src) : ACmd(src) {
	(void)src;
}
// vv copy assignment constructor
CmdInvite& CmdInvite::operator=(const CmdInvite& src) {
	if (this != &src)
		this->ACmd::operator=(src);
	return *this;
}
// vv destructor
CmdInvite::~CmdInvite() {}

/* LOGIC FUNCTIONS */
void CmdInvite::execute(User &user, Server &server) {
	// (1) checks the number of parameters
	if (getParamCount() < 2) {
		// Sends ERR_NEEDMOREPARAMS (461)
		std::string msg = ":" + server.getServerName() + " 461 " + user.getNickname() + " INVITE :Not enough parameters";
		user.reply(msg);
		return;
	}
	
	std::string targetNick = getParam(0);
	std::string channelName = getParam(1);
	Channel* channel = server.findChannel(channelName);

	// (2) if the channel doesn't exist
	if (channel == NULL) {
		// Format: ":serverName 403 targetNick channelName: No such Channel";
		// 403: ERR_NOSUCHCHANNEL
		std::string msg = ":" + server.getServerName() + " 403 " + user.getNickname() + " " + channelName + " :No such Channel";
		user.reply(msg);
		return;
	}

	// (3) if invitor user exitst in the channel
	// isUserInChannel needs to be implemented in the Channel class!
	if (!channel->isUserInChannel(user.getNickname())) {
		std::string msg = ":" + server.getServerName() + " 442 " + user.getNickname() + " :You're not on that channel";
		user.reply(msg);
		return;
	}

	// (4) if `targetNick` exists in the server
	User* targetUser = server.findUserByNick(targetNick);
	if (targetUser == NULL) {
		std::string msg = ':' + server.getServerName() + " 401 " + user.getNickname() + " " + targetNick + " :No such nick/channel";
		user.reply(msg);
		return;
	}

	// (5) if `targetNick` is already in the channel
	if (channel->isUserInChannel(targetNick)) {
		std::string msg = ":" + server.getServerName() + " 443 " + user.getNickname() + " :is already on channel";
		user.reply(msg);
		return;
	}
	// (6) Success send a message and add `targetNick` to `_invitedUsers`
	channel->addInvite(targetNick);
	//     (-) Send invite message to the given user
	std::string inviteMsg = ":" + user.getNickname() + "!" + user.getUserName() + "@" + user.getHostName() + " INVITE " + targetNick + " :" + channelName;
	targetUser->reply(inviteMsg);
	//     (-) Send RPL_INVITING (341); success message to the invitor.
	std::string rplMsg = ":" + server.getServerName() + " 341 " + user.getNickname() + " " + targetNick + " " + channelName;
	user.reply(rplMsg);
}