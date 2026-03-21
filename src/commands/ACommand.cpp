#include "commands/ACommand.hpp"
#include "User.hpp"

// getters
const std::string& getParam(size_t index);
// This function is used to check if user provides proper number of parameter
// for example, KICK command needs two parameters (channel name, nickname),
// 
size_t getParamCount() const;

std::vector<std::string> ACommand::splitBySpaces(std::string str)
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