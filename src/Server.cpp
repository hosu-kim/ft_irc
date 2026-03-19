#include "Server.hpp"

Server::Server(char **argv) : port(atoi(argv[1])), password(argv[2])
{
//	check the port number

//	...
}

Server::~Server()
{
//	should probably be empty
}


void Server::setSocket()
{
   	int opt = 1;
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
        std::cerr << "Failed to create socket." << '\n';
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        std::cerr << "Failed to set option (SO_REUSEADDR) on socket." << '\n';
    //if (write(server_fd, F_SETFL, O_NONBLOCK) == -1)
    //    std::cerr << "Failed to set option (O_NONBLOCK) on socket." << '\n';
    //bind
    //if (bind(server_fd, (struct socaddr *)&add, sizeof(add)) == -1)
    //  std::cerr << "Failed to bind socket." << '\n';
    if (listen(server_fd, SOMAXCONN) == -1)
        std::cerr << "listen() failed." << '\n';
}

void Server::run()
{
    setSocket();

    //while (true)
    //{
        //poll loop
    //}
}
