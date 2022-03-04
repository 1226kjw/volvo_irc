#include "Server.hpp"

int irc_atoi(char *str)
{
	long long int ret = 0;
	for (int i = 0; str[i] && ret >= 0; ++i)
		ret = ret * 10 + str[i] - '0';
	if (ret < 0)
		throw std::out_of_range("");
	return ret;
}

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

	Server irc_server(irc_atoi(av[ac - 2]), av[ac - 1]);

	if (irc_server.setup())
		return 1;
	irc_server.run();
}
