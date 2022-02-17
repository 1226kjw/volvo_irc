#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <cstring>
#include <cstdlib>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::vector;
using std::map;
using std::set;

#define BUFFER_SIZE 1024

#include "Server.hpp"

set<string> Client::taken = set<string>();

int main(int ac, char **av)
{
	if (ac < 3 || ac > 4)
		return 1;

	Server irc_server(atoi(av[ac - 2]), av[ac - 1]);

	if (irc_server.setup())
		return 1;
	irc_server.run();
	close(irc_server.client_fd[0].fd);
}
