#include "../include/User.hpp"
#include "../include/Server.hpp"

void check_port(char* argv1)
{
	for (int i = 0; argv1[i]; i++)
	{
		if (!isdigit(argv1[i]))
			throw std::invalid_argument("Port must be a number");
		int port = atoi(argv1);
		if (port < 1024 || port >= 49152)
			throw std::out_of_range("Port must be a number between 1024 and 49151");
	}
}

int main(int argc, char** argv) // argv[1] ... port | argv[2] ... password
{
	try
	{
		if (argc != 3)
			throw std::invalid_argument("Usage: program <port> <password>");
		check_port(argv[1]);
		Server server(argv);
		server.run();
	}
	catch (const std::invalid_argument &e)
	{
		std::cerr << "Invalid argument: " << e.what() << '\n';
		return (1);
	}
	catch (const std::out_of_range &e)
	{
		std::cerr << "Out of range: " << e.what() << '\n';
		return (1);
	}
	return (0);
}