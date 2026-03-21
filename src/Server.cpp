#include "Server.hpp"

Server::Server(char **argv) : port(atoi(argv[1])), password(argv[2])
{
    if (port <= 0 || port > MAX_PORT_NUMBER)
        throw Server::RunTimeError("Invalid port.");
    
    std::cout << "Creating a server..." << std::endl;
}

Server::~Server()
{
    std::cout << "Server shutting down..." << std::endl;
    close(server_fd);
}


void Server::setSocket()
{
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
        throw Server::RunTimeError("socket() failed.");

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        throw Server::RunTimeError("Failed to set option (SO_REUSEADDR) on socket.");
    
    if (fcntl(server_fd, F_SETFL, O_NONBLOCK) == -1)
        throw Server::RunTimeError("fcntl() failed.");
    
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
        throw Server::RunTimeError("Failed to bind socket.");
    
    if (listen(server_fd, SOMAXCONN) < 0)
        throw Server::RunTimeError("listen() failed.");

    user_poll[0].fd = server_fd;
    user_poll[0].events = POLLIN /*| POLLHUP | POLLOUT*/;
    fd_count = 1;
    std::cout << "setSocket() finished..." << std::endl;
}


void    Server::newClient(void)
{
    // GOAL: accept all pending clients and add them to poll()
    // On server_fd: one POLLIN != one client!!!!
    // There may be multiple queued 
    // while (true)
    // {
    //      int client_fd = accept(...); ------ + check for fail
    //      if (client_fd < 0)
    //          break when no more clients
    //      else -> handle client
    //          1. Set non-blocking: fcntl(client_fd, F_SETFL, O_NONBLOCK;)
    //          2. Add to poll array:
    //              user_poll[fd_count].fd = clinet_fd;
    //              user_poll[fd_count].events = POLLIN;
    //              fd_count++;
    //          3. Add to user map:
    //              users[client_fd] = User(client_fd);
    //          4. Check for fd_count >= MAX_CLIENTS -> close client, refuse connection
    // }

    int user_fd;
    struct sockaddr_in   useraddr;
    socklen_t   addrlen = sizeof(useraddr);


    while (true)
    {
        user_fd = accept(server_fd, (sockaddr *)&useraddr, &addrlen);
        if (user_fd < 0)
        {
            if (errno == EWOULDBLOCK || errno == EAGAIN)
                break;
            else
                throw Server::RunTimeError("accept() failed.");
        }
        if (fd_count >= MAX_CLIENTS)
        {
            close(user_fd);
            return;
        }
        else
        {
            fcntl(user_fd, F_SETFL, O_NONBLOCK);
            user_poll[fd_count].fd = user_fd;
            user_poll[fd_count].events = POLLIN;
            fd_count++;
            users[user_fd] = User(user_fd);
        }
    }
}

User* Server::findUserByNick(const std::string& nick)
{
    for (int i = 0; i < users.size(); i++)
    {
        if (users[i].getNickname() == nick)
            return &users[i];
    }
    return NULL;
}

void    Server::clientRequest(int i)
{
    // GOAL: read data from a clinet safely
    char buffer[1024];
    int bytes = recv(/*server_fd*/ user_poll[i].fd, buffer, sizeof(buffer), 0);
    int fd = user_poll[i].fd;

    if (bytes > 0)
    // Client sent data -> append to user buffer, parse commands
    {
        std::string message(buffer, bytes);

        users[fd].buffer += message;

        //            PARSE COMMANDS !!!!!!!!!!!!!!!!!!!
        // while (buffer contains "\r\n")   ------ size_t pos = buffer.find("\r\n");
        // {
        //      extract one line ---------- substr(0, pos);
        //      remove it from buffer   --------- erase(0, pos + 2);
        //      process it
        // }

        std::string &buf = users[fd].buffer;

        while (true)
        {
            size_t pos = buf.find("\r\n");
            if (pos == std::string::npos)
                break;
            std::string line = buf.substr(0, pos);

            buf.erase(0, pos + 2);

            if (line.empty())
                continue;

            std::vector<std::string> tokens = ACommand::splitBySpaces(line);
            if (tokens.empty())
                continue;

            std::string cmd = tokens[0];
            if (cmd == "PASS")
            {
                if (tokens.size() < 2)
                    continue;
                if (tokens[1] == password)
                    users[fd].setPassOK(true);
            }
            else if (cmd == "USER")
            {
                if (tokens.size() > 4)
                {
                    users[fd].setFullname(tokens[4]);
                    users[fd].setHasUser(true);
                }
            }
            else if (cmd == "NICK")
            {
                if (tokens.size() > 1)
                {
                    users[fd].setNickname(tokens[1]);
                    users[fd].setHasNick(true);
                }
            }
            else if (cmd == "PRIVMSG")
            {
                // 1. Validate input:

                if (tokens.size() < 3)
                {
                //      send error: ERR_NEEDMOREPARAMS
                      continue;
                }

                if (!users[fd].getRegistered())
                      return;

                // 2. Extract data:

                std::string target = tokens[1];
                std::string text = tokens[2];

                if (target[0] == '#')
                {
                //    Channel ch; // set to the right channel
                //      ->channel message (LATER)
                //    std::string msg = ":" + text;
                    //ch.broadcast(msg, users[fd]);
                // TODO later
                    continue;
                }
                
                // else -> user message

                else
                {
                    User *targetUser = findUserByNick(target);
                    if (!targetUser)
                    {
                        std::string err = ":localhost 401 " + users[fd].getNickname() + " " + target +" :No such nick\r\n";
                        send(fd, err.c_str(), err.size(), 0);
                        continue;
                    }
                    int target_fd = (*targetUser).getFd();
                    std::string msg = ":" + users[fd].getNickname() + " PRIVMSG " + target + " :" + text + "\r\n";
                    send(target_fd, msg.c_str(), msg.size(), 0);
                }
                // 3. Find user (user message):
                // User *targetUser = findUserByNick(target); ------- implement this (loop through users map)
                //         then send message: 
                //              std::string msg = ":" + users[fd].getNickName() + " PRIVMSG " + target + " :" + text + "\r\n";
                //              send(target_fd, msg.c_str(), msg.size(), 0);

                // 4. Channel case - later (use Channel::broadcast())
            }

            if (!users[fd].getRegistered() && users[fd].getPassOK() && users[fd].getHasNick() && users[fd].getHasUser())
            {
                users[fd].setRegistered(true);
                std::string welcome = ":" + std::string("localhost") + " 001 " + users[fd].getNickname() + " :Welcome to our IRC server\r\n";
                send(fd, welcome.c_str(), welcome.size(), 0);
            }
        }
    }
    else if (bytes == 0)
    {
        // disconnect client
        std::cout << "Client disconnected: " << fd << std::endl;
        removeUser(i);
    }
    else
    {
        if (errno == EWOULDBLOCK || errno == EAGAIN)
            return;
        else
            removeUser(i);
    }
}

void    Server::removeUser(int i)
{
    // GOAL: fully remove a client
    // 1. Get fd:
    int fd = user_poll[i].fd;
    // 2. Close socket:
    close(fd);
    // 3. Remove from user map:
    users.erase(fd);
    // 4. Remove from poll array - shift everything left:
    for (int j = i; j < fd_count -1; j++)
        user_poll[j] = user_poll[j + 1];
    fd_count--;
}


void Server::run()
{
    setSocket(); //add check for success..?

    while (true)
    {
//      std::cout << "entering poll loop..." << std::endl;
        if (poll(user_poll, fd_count, -1) < 0)          // change -1 to 0
            throw Server::RunTimeError("Error: poll failed.");
        std::cout << "Poll successful." << std::endl;
        //unsigned int current_size = fd_count;
        for (unsigned int i = 0; i < fd_count; i++)
        {
            std::cout << "for loop inside the poll() loop" << std::endl;
            try
            {
                if (user_poll[i].revents & POLLIN)
                {
                    if (user_poll[i].fd == server_fd)
                        newClient(); 
                    else
                        clientRequest(i); 
                }
                else if ((user_poll[i].revents & (POLLHUP | POLLERR)) /*|| (user_poll[i].revents & POLLOUT)*/)
                {
                    removeUser(i);
                    i--;
                }
            }
            catch (std::exception &e)
            {
                std::cerr << e.what() << '\n';
            }
            user_poll[i].revents = 0;
        }
    }
}
