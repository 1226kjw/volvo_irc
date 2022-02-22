#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <map>
# include <set>
# include <vector>
# include <queue>
# include <cstring>

# include <sys/types.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <poll.h>
# include <unistd.h>

# include "Client.hpp"
# include "Channel.hpp"

#define CLIENT_MAX 1000
#define BUFFER_SIZE 1024

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::map;
using std::set;
using std::vector;
using std::priority_queue;

vector<string> split(string str, char d = ' ')
{
	vector<string> ret;
	string istr;
	for (string::iterator c = str.begin(); c != str.end(); ++c)
	{
		if (istr == "" && *c == ':')
		{
			ret.push_back(str.substr(c - str.begin(), string::npos));
			break ;
		}
		if (*c != d)
			istr.push_back(*c);
		else if (istr != "")
		{
			ret.push_back(istr);
			istr = "";
		}
	}
	if (istr != "")
		ret.push_back(istr);
	return ret;
}

class Server
{
public:
	const int port;
	const string passwd;
	map<string, int> client_map;
	map<string, Channel> channel;
	map<int, Client> client;
	struct pollfd client_fd[CLIENT_MAX];
	priority_queue<int> available_index;

	struct sockaddr_in server_addr, client_addr;
	socklen_t clientaddr_len;
	int server_socket;
	char buf[BUFFER_SIZE + 1];

	int max_index;
	int idx;

	Server(int port, string pw): port(port), passwd(pw), clientaddr_len(sizeof(client_addr)) {}
	~Server()
	{
		for (int i = 0; i < CLIENT_MAX; ++i)
			if (client_fd[i].fd != -1)
				close(client_fd[i].fd);
	}

	int setup()
	{
		if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			cerr << "Cannot create a socket" << endl;
			return 1;
		}
		int op = 1;
		setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op));
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
		client_fd[0].fd = server_socket;
		client_fd[0].events = POLLIN;
		for (int i = 1; i < CLIENT_MAX; ++i)
		{
			client_fd[i].fd = -1;
			available_index.push(-i);
		}
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
				// cout << "ip  : " << inet_ntoa(client_addr.sin_addr) << endl;
				// cout << "port: " << ntohs(client_addr.sin_port) << endl;
				cout << "client accepted on fd " << client_socket << endl;
				idx = -available_index.top();
				available_index.pop();
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
					if (!(client_fd[i].revents & POLLIN))
						continue;
					--poll_ret;
					int recv_size = recv(client_fd[i].fd, buf,sizeof(buf), 0);
					if (recv_size < 0)
					{
						cerr << "recv error" << endl;
						return 1;
					}
					else if (recv_size == 0)
					{
						close(client[i].fd);
						cout << "byebye " << client[i].nickname << endl;
						cout << "index " << i << " is available" << endl;
						client_map.erase(client[i].nickname);
						for (map<string, Channel>::iterator mitr = channel.begin(); mitr != channel.end(); ++mitr)
							mitr->second.member.erase(i);
						client_fd[i].events = 0;
						client_fd[i].fd = -1;
						available_index.push(-i);
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

	// client[i].fd == client_fd[i].fd
	// i != client[i].fd
	void cmd(int i)
	{
		cout << "from " << i << ':' << client[i].msg;
		vector<string> tok = split(client[i].msg);
		if (tok.back().back() == '\n')
			tok.back().pop_back();
		
		if (tok.size() == 0)
			return ;
		
		string command = tok[0];
		vector<string> arg(tok.begin() + 1, tok.end());


		if (!client[i].is_registered)
		{
			if (!client[i].is_authenticated)
			{
				if (command == "PASS")
					client[i].authenticate(passwd, arg);
				else
					send(client[i].fd, "authenticate first\n", 20, 0);
			}
			else if (command == "PASS")
				send(client[i].fd, "already authenticated\n", 23, 0);
			else if (command == "NICK")
				client[i].nick(client_map, arg);
			else if (command == "USER")
				client[i].user(arg);
			else
				send(client[i].fd, "register first\n", 16, 0);
			client[i].msg = "";
			return ;
		}
		
		if (command == "JOIN")
		{
			if (arg.size() == 1 && arg[0] == "0")
			{
				for (map<string, Channel>::iterator itr = channel.begin(); itr != channel.end(); ++itr)
					itr->second.member.erase(i);
			}
			else
				for (vector<string>::iterator itr = arg.begin(); itr != arg.end(); ++itr)
				{
					if (itr->front() != '#')
					{
						cout << "///////////" << endl;
						continue;
					}

					string channel_name(itr->begin() + 1,itr->end());

					if (channel.find(channel_name) == channel.end())
						channel[channel_name] = Channel();
					channel[channel_name].member.insert(i);
				}
		}
		else if (command == "KICK")
		{
			
		}
		else if (command == "PART")
		{
			
		}
		else if (command == "PRIVMSG")
		{
			if (arg.size() != 2)
			{
				send(client[i].fd, "invalid num of args\n", 21, 0);
			}
			else if (arg[0][0] == '#')
			{
				cout << arg[0].substr(1, string::npos) << endl;
				if (channel.find(arg[0].substr(1, string::npos)) != channel.end())
				{
					for (set<int>::iterator itr = channel[arg[0].substr(1, string::npos)].member.begin(); itr != channel[arg[0].substr(1, string::npos)].member.end(); ++itr)
						send(client[*itr].fd, client[i].msg.c_str(), client[i].msg.size(), 0);
				}
				else
					send(client[i].fd, (string("channel ") + arg[0] + " not found\n").c_str(), arg[0].size() + 21, 0);
			}
			else
			{
				if (client_map.find(arg[0]) != client_map.end())
					send(client[client_map[arg[0]]].fd, client[i].msg.c_str(), client[i].msg.size(), 0);
				else
					send(client[i].fd, "user not found\n", 16, 0);
			}
		}
		else if (command == "NOTICE")
		{
			
		}
		else if (command == "QUIT")
		{
			
		}
		client[i].msg = "";
	}
};

#endif