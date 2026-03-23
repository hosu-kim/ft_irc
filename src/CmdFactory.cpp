#include "CmdFactory.hpp"

std::vector<std::string> CmdFactory::splitBySpaces(std::string str)
{
	std::vector<std::string> result;
	size_t	start = 0, end = 0;

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