/*
TODOS:
	# Data structure and configuration of properties (member variables).
	[Done] 1. Declare member variables: channel name, channel password(key), topic
		2. User management: list of Users... std::vector<User*> or std::map<int, User*>
			- operator list: identify users with privilige separately.
		3. channel modes: i(invite-only), t(topic limit), k(password needed), l(user limit)
	
	# methods implementation
		1. Join-leave: 
		   - [DONE]joinUser(User* user, std::string password);
		   - [DONE]leaveUser(User* user);
		2. permission control(add-remove):
		   - [DONE] addOperator(User* user)
		   - [DONE] removeOperator(User* user);
		3. message broadcast: except the sender, all Users receive the messages.
		   - [DONE]broadcast(std::string msg, User* exceptUser)
		4. mode setup:
		   - [DONE]setMode()
*/

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
#include <map> // std::map
#include <set> // std::set
#include <exception>
#include <sstream>

#include "User.hpp"

class Channel {
		private:
		std::string _channelName;
		std::string _channelPassword;
		std::string _channelTopic;
		// // for the channel mode `l`(user limit)
		// if _userLimit is 0, unlimited user entry allowed
		int _userLimit;
		/* I could use std::vector also, but when we search and kick a specific user,
		   it's not effetive than std::map
		     1. we have to search through users from beginning to end. that causes slow operation...
		     2. To find duplicate users, every time we have to iterate through them 😢 */
		//       vv: nickname vv: user instance
		std::map<std::string, User*> _channelMembers;
		/* Key features of std::set
		   1. ***Duplicates not allowed***: even if the same value is entered twice,
		                                    only one is stored.
		   2. Sorts elements in ascending order
		*/
		std::set<std::string> _channelOperators;
		// Mode flags
		bool _isInviteOnly; // i mode
		bool _isTopicRestricted; // t mode
		bool _hasKey;
		std::set<std::string> _invitedUsers;

	public:
		/* Orthodox Canonical Form */
		// The constructor sets up _channelName and initializes member variables.
		Channel();
		Channel(std::string channelName, std::string channelPassword, std::string channelTopic, int userLimit);
		Channel(const Channel& src);
		Channel& operator=(const Channel& src);
		~Channel();
		//======================================================================
		/* Member Functions */
		int joinUser(User* user, std::string password);
		int removeUser(User* user);
		size_t getMemberCount() const;
		int addOperator(User* user);
		int removeOperator(User* user);

		/* Except the sender(exceptUser), all members receive the message*/
		void broadcast(std::string message, User* exceptUser);

		// mode change member functions
		User* findUserByNick(std::string nickname);
		bool stringToInt(const std::string& str, int& result);
		int setMode(char mode, char op, std::string value, User* setter);

		class IRCException : public std::exception {
			private:
				int _errorCode;
				std::string _message;

			public:
				IRCException(int code, const std::string& msg)
				: _errorCode(code), _message(msg) {}

				virtual ~IRCException() throw() {}

				virtual const char* what() const throw() {
					return _message.c_str();
				}

				int getErrorCode() const {
					return _errorCode;
				}
 		};
};
