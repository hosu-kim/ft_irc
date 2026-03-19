#include "../include/Server.hpp"

Server::Server(char **argv) : port(atoi(argv[1])), password(argv[2])
{
    if (port <= 0 || port > MAX_PORT_NUMBER)
        throw Server::RunTimeError("Invalid port.");
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
    //if (write(server_fd, F_SETFL, O_NONBLOCK) == -1) --------- don't know how this works
    
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
        throw Server::RunTimeError("Failed to bind socket.");
    
    if (listen(server_fd, SOMAXCONN) < 0)
        throw Server::RunTimeError("listen() failed.");

    memset(user_poll, 0, sizeof(user_poll));
    user_poll[0].fd = server_fd;
    user_poll[0].events = POLLIN | POLLHUP | POLLOUT;
    fd_count = 1;
}

void Server::run()
{
    setSocket(); //add check for success

    while (true)
    {
        if (poll(user_poll, fd_count, -1) < 0)
            throw Server::RunTimeError("Error: poll failed.");
        unsigned int current_size = fd_count;
        for (unsigned int i = 0; i < current_size; i++)
        {
            try
            {
                if (user_poll[i].revents & POLLIN)
                {
                    if (user_poll[i].fd == server_fd)
                        new_client(); // define this + put to header
                    else
                        client_request(i); // define this + put to header
                }
                else if ((user_poll[i].revents & POLLHUP) || (user_poll[i].revents & POLLOUT))
                    remove_user(); // define this + put to header
            }
            catch (std::exception &e)
            {
                std::cerr << e.what() << '\n';
            }
        }
    }
}
