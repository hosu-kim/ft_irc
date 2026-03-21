#pragma once

#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <vector>

#include "User.hpp"
#include "Server.hpp"
#include "Channel.hpp"

// Parser logic:
// DONE 1. Split by spaces
//      2. If ":" appears:
//            -> everything after = ONE param
//      Example:
//          Input: USER test 0 * :Real Name
//          Output: ["USER", "test", "0", "*", "Real Name"]
// Commands to handle:
//      Priority:
//          PASS, NICK, USER
//      After:
//          PRIVMSG
//      Last:
//          JOIN

// Example: PRIVMSG #chan :hello world


class ACommand
{
    private:
        // e.g., KICK #channel target_user
        //       cmd: "NICK"
        std::string                 _cmd;
        //       params: ["#channel", "target_user"]
        std::vector<std::string>    _params;

    protected:
        // getters
        const std::string& getParam(size_t index);
        size_t getParamCount() const;

    public:
        ACommand(){};

        static std::vector<std::string> splitBySpaces(std::string str); // is static necessary?

  //      void    commandDispatcher(std::string cmd);
};