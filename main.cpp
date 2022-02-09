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

#define CLIENT_MAX 1000
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
		return 1;

	int port = atoi(av[ac - 2]);
	string password = av[ac - 1];

	map<string, int> client_map;
	struct pollfd client[CLIENT_MAX];
	map<string, set<string>> channel;

	struct sockaddr_in server_addr, client_addr;
	socklen_t clientaddr_len = sizeof(client_addr);
	int server_socket;
	char buf[BUFFER_SIZE + 1];

	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		cerr << "Cannot create a socket" << endl;
		return 1;
	}
	int op = 1;
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(int));
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(port);

	if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		cerr << "Cannot bind name to socket" << endl;
		return 1;
	}
	if (listen(server_socket, 5) < 0)
	{
		cerr << "Listen error" << endl;
		return 1;
	}

	cout << "Server on" << endl;
	client[0].fd = server_socket;
	client[0].events = POLLIN;
	int count = 1;
	for (int i = 1; i < CLIENT_MAX; ++i)
		client[i].fd = -1;

	while (1)
	{
		int poll_ret = poll(client, count, -1);
		if (client[0].revents & POLLIN)
		{
			int client_socket;
			bzero(&client_addr, sizeof(client_addr));
			if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &clientaddr_len)) < 0)
			{
				cerr << "accept error" << endl;
				return 1;
			}
			cout << "ip  : " << inet_ntoa(client_addr.sin_addr) << endl;
			cout << "port: " << ntohs(client_addr.sin_port) << endl;
			cout << "accepted on fd " << client_socket << endl;
			client[count].fd = client_socket;
			client[count++].events = POLLIN;
		}
		else
		{
			for (int i = 1; i < count && poll_ret; ++i)
			{
				if (client[i].revents & POLLIN)
				{
					--poll_ret;
					int recv_size = 0;
					if ((recv_size = recv(client[i].fd, buf,sizeof(buf), 0)) < 0)
					{
						cerr << "recv error" << endl;
						return 1;
					}
					else if (recv_size == 0)
					{
						close(client[i].fd);
						client[i].events = 0;
						client[i].fd = -1;
						--count;
					}
					else
					{
						buf[recv_size] = 0;
						cout << "from fd:" << client[i].fd << endl;
						cout << "receive:" << buf << endl;
					}
				}
			}
		}
	}
	close(client[0].fd);
}
