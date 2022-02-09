#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <cstring>
#include <cstdlib>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>

#define BUFFER_SIZE 1024

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::vector;
using std::map;
using std::set;

int main(int ac, char **av)
{
	if (ac < 3 || ac > 4)
	{
		return 1;
	}
	int port = std::stoi(av[ac - 2]);
	string password = av[ac - 1];

	int listen_socket, server_socket, clientaddr_len, recv_size;
	struct hostend *h;
	struct sockaddr_in serveraddr, clientaddr;
	char buf[BUFFER_SIZE + 1];
	
	if ((listen_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		cerr << "Cannot create a socket" << endl;
		return 1;
	}
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(port);

	if (bind(listen_socket, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
	{
		cerr << "Cannot bind name to socket" << endl;
		return 1;
	}
	listen(listen_socket, 5);
	clientaddr_len = sizeof(clientaddr);
	if ((server_socket = accept(listen_socket, (struct sockaddr*)&clientaddr, (socklen_t*)&clientaddr_len)) < 0)
	{
		cerr << "Cannot accept client" << endl;
		return 1;
	}
	cout << "Connected" << endl;
	struct pollfd a;
	vector<struct pollfd> fds;
	fds.push_back(a);
}
