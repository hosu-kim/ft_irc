#include "CmdNICK.hpp"

void CmdNick::execute(User &user, Server &server)
{
	/*
	1. Check if the nickname parameter is missing
	*/
	if (getParamCount() != 1)
	{
		std::string msg = server.getServerName() + " 431 " + server.getUserName() + " :Nickname not given. Please provide us with your nickname.";
		user.reply(msg);
		return;
	}

	/*
	2. Get the requested nickname.
	*/
	std::string nick = getParam(1);
	
	/*
	3. Check for invalid characters in the nickname
	*/
	// find_first_of: - returns the index of the first character that matches any character in the given string
	//                - C++98 standard library function(<string>)
	size_t pos = nick.find_first_of(FORBIDDEN_CHARS);
	if (pos != std::string::npos) {
		std::string msg = server.getServerName() + " 432 " + server.getUserName() + " :invalid nickname";
		user.reply(msg);
		return;
	}

	/*
	4. Check if the nickname is already taken by someone else in the server
	*/
	else 
	{
		//set nickname
		user.setNickname(nick);
		user.setHasNick(true);

		std::string temp1 = "Nickname set successfully. \r\n";
		std::cout << temp1 << std::endl << "NicknameOK is: " << user.getHasNick() << std::endl;
		std::cout << "Nickname is: " << user.getNickname() << std::endl;
	}
	std::string nickMsg = ": user " + user.getUserName() + " NICK " + user.getNickname() + " @ " + user.getHostName() + "\r\n";
	// send()
}

/*
FUNCTION execute(user, server):

    // 1. Check if the nickname parameter is missing
    IF parameter count is NOT equal to 1:
        SEND ERR_NONICKNAMEGIVEN (431) to user
        RETURN
        
    // 2. Get the requested nickname (It's the first parameter, not a channel name!)
    SET requested_nickname = GET parameter at index 0
    
    // 3. Check for invalid characters in the nickname
    FOR EACH character IN requested_nickname:
        IF character is in FORBIDDEN_CHARS:
            SEND ERR_ERRONEUSNICKNAME (432) to user
            RETURN // Stop here if it's invalid!
            
    // 4. Check if the nickname is already taken by someone else in the server
    IF server.isNicknameInUse(requested_nickname) is TRUE:
        SEND ERR_NICKNAMEINUSE (433) to user
        RETURN // Stop here if it's taken!
        
    // 5. Success! Apply the new nickname
    SET user's nickname to requested_nickname
    SET user's 'hasNick' status to TRUE
    
    // 6. Announce the NICK change
    FORMAT success_message = ":<old_nick/user> NICK <new_nick>"
    SEND success_message to the user (and others who need to know)

END FUNCTION
*/