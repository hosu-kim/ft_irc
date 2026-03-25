#include "commands/ACmd.hpp"
#include "User.hpp"

// ORTHODOX CANONICAL FORM
ACmd::ACmd() {}
ACmd::ACmd(std::string cmd, std::vector<std::string> params)
	: _cmd(cmd), _params(params) {}
ACmd::ACmd(const ACmd& src) : _cmd(src._cmd), _params(src._params) {
}
ACmd& ACmd::operator=(const ACmd& src) {
	if (this != &src) {
		this->_cmd = src._cmd;
		this->_params = src._params;
	}
	return *this;
}
ACmd::~ACmd() {}

// getters
// This function is used to check if user provides proper number of parameter
// for example, KICK command needs two parameters (channel userName, nickname),
// 
size_t ACmd::getParamCount() const {
	return this->_params.size();
}

const std::string& ACmd::getParam(size_t index) {
	if (index >= getParamCount()) {
		throw std::out_of_range("Parameter index out of range");
	}
	return this->_params[index];
}


// std::vector<std::string> ACmd::splitBySpaces(std::string str)
// {
// 	std::vector<std::string> result;
// 	size_t	start = 0, end = 0;

// 	while ((start = str.find_first_not_of(' ', end)) != std::string::npos)
// 	{
// 		if (str[start] == ':')
// 		{
// 			start++;
// 			end = str.size();
// 		}
// 		else
// 		{
// 			end = str.find(' ', start);
// 		}
// 		result.push_back(str.substr(start, end - start));
// 	}
// 	return result;
// }


