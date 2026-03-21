#include "commands/ACommand.hpp"
#include "User.hpp"

std::vector<std::string> Command::splitBySpaces(std::string str)
{
	std::vector<std::string> result;
	int	start = 0, end = 0;

	while ((start = str.find_first_not_of(' ', end)) != std::string::npos)
    {
        if (str[start] == ':')
        {
            start++;
            end = str.size();
        }
        else
        {
            end = str.find(' ', start);
        }
        result.push_back(str.substr(start, end - start));
    }
    return result;
}


/*
void    Command::commandDispatcher(std::string cmd)
{
    if (cmd == "PASS")
        handlePass(User &, params);
    else if (cmd == "NICK")
        handleNickname();
    else if (cmd == "USER")
        handleUser();
}*/