/*
TODOS:
	# Data structure and configuration of properties (member variables).
	[Done] 1. Declare member variables: channel name, channel password(key), topic
		2. User management: list of Users... std::vector<User*> or std::map<int, User*>
			- operator list: identify users with privilige separately.
		3. channel modes: i(invite-only), t(topic limit), k(password needed), l(user limit)
	
	# methods implementation
		1. Join-leave: joinUser(User* user); leaveUser(User* user);
		2. permission control(add=remove): addOperator(User* user), removeOperator(User* user);
		3. message broadcase: broadcase(std::string msg, User* exceptUser)
			except the sender, all Users receive the messages.
		4. mode change: setMode(), checkMode()



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

#include "User.hpp"

class Channel {
	public:
		/* Orthodox Canonical Form */
		// The constructor sets up _channelName and initializes member variables.
		Channel();
		Channel(std::string channelName);
		Channel(const Channel& src);
		Channel& operator=(const Channel& src);
		~Channel();
		//======================================================================
		/* Member Functions */
		int joinUser(User* user, std::string password);

	private:
		std::string _channelName;
		std::string _channelPassword;
		std::string _channelTopic;
		/* I could use std::vector also, but when we search and kick a specific user,
		   it's not effetive than std::map
		     1. we have to search through users from beginning to end. that causes slow operation...
		     2. To find duplicate users, every time we have to uterate through them 😢 */
		//       vv: nickname vv: user instance
		std::map<std::string, User*> _channelMembers;
};