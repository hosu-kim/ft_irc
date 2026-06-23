#include "CmdFactory.hpp"
#include <cctype>
#include "commands/CmdPASS.hpp"
#include "commands/CmdNICK.hpp"
#include "commands/CmdUSER.hpp"
#include "commands/CmdJOIN.hpp"
#include "commands/CmdINVITE.hpp"
#include "commands/CmdKICK.hpp"
#include "commands/CmdMODE.hpp"
#include "commands/CmdPING.hpp"
#include "commands/CmdPONG.hpp"
#include "commands/CmdPRIVMSG.hpp"
#include "commands/CmdQUIT.hpp"
#include "commands/CmdTOPIC.hpp"
#include "commands/CmdPART.hpp"


// Private constructor and destructor
CmdFactory::CmdFactory() {}

/**
 * Splits a string by spaces, considering the IRC trailing parameter (':')
 * 
 * Note: The carriage returns ('\r', '\n') are removed in the server class in advance
 */
std::vector<std::string> CmdFactory::splitBySpaces(const std::string& input) {
	/* 
	 * 1. Create a vector of strings to store tokens
	 */
	std::vector<std::string> tokens;

	/* 
	 * 2. Loop through the input string from start to end (using index or pointer)
	 */
	std::string::size_type i = 0, n = input.size(); // size_type: size_t for cross-platform type stability
	while (i < n) {
		// If current char is a space (' '), skip it and move forward
		while (i < n && input[i] == ' ')
			++i;

		if (i >= n)
			break;
		
		// If current char is a colon (':') => user message
		if (input[i] == ':') {
			// Extract everything from the current position to the end of the string as one token 
			// And add it to the tokens vector
			++i;
			tokens.push_back(input.substr(i));
			break;
		}

		// Regular tokens process
		size_t j = input.find(' ', i);
		if (j == std::string::npos) {
			tokens.push_back(input.substr(i)); // Last token;
			break;
		} else {
			tokens.push_back(input.substr(i, j - i));
			i = j + 1;
		}
	}
	return tokens;
}

/**
 * Analyzes tokens and creates the appropriate command object
 */
ACmd* CmdFactory::createCommand(const std::vector<std::string>& tokens) {
	if (tokens.empty()) {
		return NULL;
	}

	std::string cmdName = tokens[0];
	for (size_t i = 0; i < cmdName.size(); ++i) {
		cmdName[i] = std::toupper(cmdName[i]);
	}

	std::vector<std::string> params(tokens.begin() + 1, tokens.end());

	if (cmdName == "PASS") {
		return new CmdPass(cmdName, params);
	} else if (cmdName == "NICK") {
		return new CmdNick(cmdName, params);
	} else if (cmdName == "USER") {
		return new CmdUser(cmdName, params);
	} else if (cmdName == "JOIN") {
		return new CmdJoin(cmdName, params);
	} else if (cmdName == "MODE") {
		return new CmdMode(cmdName, params);
	} else if (cmdName == "INVITE") {
		return new CmdInvite(cmdName, params);
	} else if (cmdName == "KICK") {
		return new CmdKick(cmdName, params);
	} else if (cmdName == "PART") {
		return new CmdPart(cmdName, params);
	} else if (cmdName == "PING") {
		return new CmdPing(cmdName, params);
	} else if (cmdName == "PONG") {
		return new CmdPong(cmdName, params);
	} else if (cmdName == "PRIVMSG") {
		return new CmdPrivmsg(cmdName, params);
	} else if (cmdName == "QUIT") {
		return new CmdQuit(cmdName, params);
	} else if (cmdName == "TOPIC") {
		return new CmdTopic(cmdName, params);
	}

	return NULL;
}