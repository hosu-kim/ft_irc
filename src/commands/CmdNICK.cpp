#include "CmdNICK.hpp"

void CmdNick::execute(User &user, Server &server)
{
    if (getParamCount() != 1)
    {
        std::string msg = server.getServerName() + " 431 " + server.getUserName() + " :Nickname not given. Please provide us with your nickname.\r\n";
        user.reply(msg);
        return;
    }
    //other checks:
    //  Already in use
    //          TODO

    std::string channelName = getParam(0);
    std::string nick = (getParamCount() >= 2) ? getParam(1) : "";
    
    // Invalid character
    if (getParamCount() == 1)
    {
        for (unsigned int i = 0; i < std::string(FORBIDDEN_CHARS).size(); ++i)
        {
            if (nick.find(FORBIDDEN_CHARS[i]) != std::string::npos)
            {
                std::cout << "There is a forbidden character in your nickname: " << FORBIDDEN_CHARS[i] << "\r\n";
                //send error message
            }
        }
    }
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