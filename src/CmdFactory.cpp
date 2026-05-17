#include "CmdFactory.hpp"

// Private constructor and destructor (Keep them empty)
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
		// 2a. If current char is a space (' '), skip it and move forward
		while (i < n && input[i] == ' ')
			++i;

		if (i >= n)
			break;
		
		// 2b. If current char is a colon (':') => user message
		if (input[i] == ':') {
			// 2b-1. Extract everything from the current position to the end of the string as one token 
			//       And add it to the tokens vector
			++i;
			tokens.push_back(input.substr(i));
			break;
		}

		// 2c. Regular tokens process
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

	//
	//        C. Otherwise (regular character):
	//           - Find the position of the 'next space' from the current position
	//           - If a space is found: substring from current to next space, add to vector, and update position
	//           - If no space is found: substring the remaining string, add to vector, and break
	//    }
	
	// 4. Return the tokens vector
}

/**
 * @brief Analyzes tokens and creates the appropriate command object
 */
ACmd* CmdFactory::createCommand(const std::vector<std::string>& tokens) {
	// 1. If tokens vector is empty, return NULL
	
	// 2. tokens[0] is the command name (e.g., "nick", "JOIN")
	//    - Convert tokens[0] to UPPERCASE because IRC commands are case-insensitive
	
	// 3. Extract parameters (from tokens[1] to the end) into a separate vector (e.g., params)
	
	// 4. Map the uppercase command name and dynamically allocate (new) the matching object
	//    if (cmdName == "PASS")    return new CmdPass(cmdName, params);
	//    if (cmdName == "NICK")    return new CmdNick(cmdName, params);
	//    if (cmdName == "USER")    return new CmdUser(cmdName, params);
	//    ... (Map all other commands like JOIN, INVITE, KICK, MODE, PING, PRIVMSG, QUIT, TOPIC)
	
	// 5. If no commands match, return NULL (Unknown command)
}