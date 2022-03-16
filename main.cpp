#include "Server.hpp"

using std::cout;
using std::cin;
using std::endl;

int main(int ac, char **av)
{
	if (ac < 3 || ac > 4)
		return 1;
	for (int i = 0; av[ac - 2][i]; ++i)
		if (!isdigit(av[ac - 2][i]))
		{
			cout << "invalid character in port" << endl;
			return 1;
		}

	Server irc_server(atoi(av[ac - 2]), av[ac - 1]);

	if (irc_server.setup())
		return 1;
	irc_server.run();
}
