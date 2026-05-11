#include "CmdPing.hpp"

void CmdPing::execute(User& user, Server& server) {
	/*
	Check if the params container is empty (or size is 0)
	*/
	if (this->_params.empty()) {
		std::string msg = ":" + server.getServerName() + " 409 " + user.getNickname() + " :No origin specified";
		user.reply(msg);
		return;
	}

	// [Step 2] Normal Process: Grab the token sent by the client 🔍
	received_token = Get the first element of params (index 0)

	// [Step 3] Build the PONG response message 🛠️
	// Format: ":<server_name> PONG <server_name> :<received_token>\r\n"
	response_message = Assemble ":" + server's name + " PONG " + server's name + " :" + received_token + "\r\n"

	// [Step 4] Send it back to the client 🚀
	Send the response_message to the user!
}