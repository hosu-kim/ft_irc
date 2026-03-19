#include "User.hpp"
#include "Server.hpp"

int main(int argc, char** argv) // argv[1] ... port | argv[2] ... password
{
	try
	{
		if (argc != 3)
			throw Server::WrongArgumentCount(argc);
		Server server(argv);
//		server.run();
	}
	catch (const Server::WrongArgumentCount &e)
	{
		std::cerr << e.what() << '\n';
		return (1);
	}
	return (0);
}