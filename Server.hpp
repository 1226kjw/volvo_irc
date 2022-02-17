#ifndef SERVER_HPP
# define SERVER_HPP

# include <map>
# include <set>
# include <poll.h>

# include "Client.hpp"
# include "Channel.hpp"

#define CLIENT_MAX 1000
#define BUFFER_SIZE 1024

using std::string;
using std::map;
using std::set;

class Server
{
public:
	const int port;
	const string passwd;
	map<string, set<int> > channel;
	map<string, int> client_map;
	map<int, Client> client;
	struct pollfd client_fd[CLIENT_MAX];

	struct sockaddr_in6 server_addr, client_addr;
	socklen_t clientaddr_len;
	int server_socket;
	char buf[BUFFER_SIZE + 1];

	int max_index;
	int idx;

	Server(int port, string pw): port(port), passwd(pw), clientaddr_len(sizeof(client_addr)) {}
	int setup()
	{
		if ((server_socket = socket(AF_INET6, SOCK_STREAM, 0)) < 0)
		{
			cerr << "Cannot create a socket" << endl;
			return 1;
		}
		int op = 1;
		setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op));
		bzero(&server_addr, sizeof(server_addr));
		server_addr.sin6_family = AF_INET6;
		server_addr.sin6_addr = in6addr_any;
		server_addr.sin6_port = htons(port);
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
		client_fd[0].fd = server_socket;
		client_fd[0].events = POLLIN;
		for (int i = 1; i < CLIENT_MAX; ++i)
			client_fd[i].fd = -1;
		return 0;
	}
	int run()
	{
		max_index = 1;
		idx = 1;
		while (1)
		{
			int poll_ret = poll(client_fd, max_index, -1);
			if (client_fd[0].revents & POLLIN)
			{
				int client_socket;
				bzero(&client_addr, sizeof(client_addr));
				if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &clientaddr_len)) < 0)
				{
					cerr << "accept error" << endl;
					return 1;
				}
				char ip[128];
				inet_ntop(AF_INET6, &client_addr.sin6_addr, ip, 128);
				cout << "ip  : " << ip << endl;
				cout << "port: " << ntohs(client_addr.sin6_port) << endl;
				cout << "accepted on fd " << client_socket << endl;
				for (idx = 1; client_fd[idx].fd != -1 && idx < max_index; ++idx) ; /////////////////////////////////////////
				cout << "idx : " << idx << endl << endl;
				if (idx != max_index)
				{
					client_fd[idx].fd = client_socket;
					client_fd[idx].events = POLLIN;
				}
				else
				{
					client_fd[max_index].fd = client_socket;
					client_fd[max_index++].events = POLLIN;
				}
				client[idx] = Client(idx, client_socket);
			}
			else
			{
				for (int i = 1; i < max_index && poll_ret; ++i)
				{
					if (client_fd[i].revents & POLLIN)
					{
						--poll_ret;
						int recv_size = recv(client_fd[i].fd, buf,sizeof(buf), 0);
						if (recv_size < 0)
						{
							cerr << "recv error" << endl;
							return 1;
						}
						else if (recv_size == 0)
						{
							close(client_fd[i].fd);
							client_fd[i].events = 0;
							client_fd[i].fd = -1;
						}
						else
						{
							buf[recv_size] = 0;
							client[i].feed(buf);
							if (client[i].msg.back() == '\n')
								cmd(i);
						}
					}
				}
			}
		}
	}
	void cmd(int i)
	{
		size_t pos = client[i].msg.find(' ');
		string command = client[i].msg.substr(0, pos);
		if (pos != string::npos)
			while (client[i].msg[pos] == ' ')
				++pos;
		string arg = pos == string::npos ? "" : client[i].msg.substr(pos, string::npos);

		client[i].msg = "";

		if (!client[i].is_registered)
		{
			if (!client[i].is_authenticated)
			{
				if (command == "PASS")
					client[i].authenticate(passwd, arg);
				else
					send(client[i].fd, "authenticate first\n", 20, 0);
			}
			else if (command == "NICK")
				client[i].nick(client_map, arg);
			else if (command == "USER")
				client[i].user(arg);
			else
				send(client[i].fd, "register first\n", 16, 0);
			return ;
		}
		
		if (command == "JOIN")
		{
			
		}
		else if (command == "PRIVMSG")
		{

		}
	}
};

#endif