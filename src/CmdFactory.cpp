#include "CmdFactory.hpp"

// Private constructor and destructor (Keep them empty)
CmdFactory::CmdFactory() {}

/**
 * @brief Splits a string by spaces, considering the IRC trailing parameter (':')
 */
std::vector<std::string> CmdFactory::splitBySpaces(const std::string& str) {
    // 1. Create a vector of strings to store tokens
    // 2. Trim trailing carriage returns ('\r', '\n') from the input string
    
    // 3. Loop through the string from start to end (using index or pointer)
    //    while (not at the end of the string) {
    //        
    //        A. If current char is a space (' '), skip it and move forward
    //
    //        B. If current char is a colon (':') -> CRITICAL IRC RULE!
    //           - Extract everything from the current position to the end of the string as ONE token
    //           - Add it to the tokens vector
    //           - Break out of the loop immediately
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