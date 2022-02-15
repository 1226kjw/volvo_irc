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

int main(int ac, char **av)
{
	if (ac < 3 || ac > 4)
		return 1;

	Server irc_server(atoi(av[ac - 2]), av[ac - 1]);

	if (irc_server.setup())
		return 1;

	// struct sockaddr_in6 server_addr, client_addr;
	// socklen_t clientaddr_len = sizeof(client_addr);
	// int server_socket;
	// char buf[BUFFER_SIZE + 1];

	// if ((server_socket = socket(AF_INET6, SOCK_STREAM, 0)) < 0)
	// {
	// 	cerr << "Cannot create a socket" << endl;
	// 	return 1;
	// }
	// int op = 1;
	// setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op));
	// bzero(&server_addr, sizeof(server_addr));
	// server_addr.sin6_family = AF_INET6;
	// server_addr.sin6_addr = in6addr_any;
	// server_addr.sin6_port = htons(irc_server.port);
	// server_addr.sin_family = AF_INET;
	// server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	// server_addr.sin_port = htons(irc_server.port);

	// if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	// {
	// 	cerr << "Cannot bind name to socket" << endl;
	// 	return 1;
	// }
	// if (listen(server_socket, 5) < 0)
	// {
	// 	cerr << "Listen error" << endl;
	// 	return 1;
	// }

	cout << "Server on" << endl;
	// irc_server.client_fd[0].fd = server_socket;
	// irc_server.client_fd[0].events = POLLIN;

	// for (int i = 1; i < CLIENT_MAX; ++i)
	// 	irc_server.client_fd[i].fd = -1;

	irc_server.run();
	// while (1)
	// {
	// 	int poll_ret = poll(irc_server.client_fd, max_client_num, -1);
	// 	if (irc_server.client_fd[0].revents & POLLIN)
	// 	{
	// 		int client_socket;
	// 		bzero(&irc_server.client_addr, sizeof(irc_server.client_addr));
	// 		if ((client_socket = accept(irc_server.server_socket, (struct sockaddr *)&irc_server.client_addr, &irc_server.clientaddr_len)) < 0)
	// 		{
	// 			cerr << "accept error" << endl;
	// 			return 1;
	// 		}
	// 		// cout << "ip  : " << inet_ntop(AF_INET6, &client_addr.sin6_addr, 0, 0) << endl;
	// 		cout << "port: " << ntohs(irc_server.client_addr.sin6_port) << endl;
	// 		cout << "accepted on fd " << client_socket << endl << endl;
	// 		for (idx = 1; irc_server.client_fd[idx].fd != -1 && idx < max_client_num; ++idx) ;
	// 		if (idx != max_client_num)
	// 		{
	// 			irc_server.client_fd[idx].fd = client_socket;
	// 			irc_server.client_fd[idx].events = POLLIN;
	// 		}
	// 		else
	// 		{
	// 			irc_server.client_fd[max_client_num].fd = client_socket;
	// 			irc_server.client_fd[max_client_num++].events = POLLIN;
	// 		}
	// 	}
	// 	else
	// 	{
	// 		for (int i = 1; i < max_client_num && poll_ret; ++i)
	// 		{
	// 			if (irc_server.client_fd[i].revents & POLLIN)
	// 			{
	// 				--poll_ret;
	// 				int recv_size = recv(irc_server.client_fd[i].fd, irc_server.buf,sizeof(irc_server.buf), 0);
	// 				if (recv_size < 0)
	// 				{
	// 					cerr << "recv error" << endl;
	// 					return 1;
	// 				}
	// 				else if (recv_size == 0)
	// 				{
	// 					close(irc_server.client_fd[i].fd);
	// 					irc_server.client_fd[i].events = 0;
	// 					irc_server.client_fd[i].fd = -1;
	// 				}
	// 				else
	// 				{
	// 					irc_server.buf[recv_size] = 0;
	// 					cout << "from fd:" << irc_server.client_fd[i].fd << endl;
	// 					cout << "receive:" << irc_server.buf << endl;
	// 					send(irc_server.client_fd[i].fd, "echo:", 5, 0);
	// 					send(irc_server.client_fd[i].fd, irc_server.buf, recv_size, 0);
	// 				}
	// 			}
	// 		}
	// 	}
	// }
	close(irc_server.client_fd[0].fd);
}
