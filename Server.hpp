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

	struct sockaddr_in server_addr, client_addr;
	socklen_t clientaddr_len;
	int server_socket;
	char buf[BUFFER_SIZE + 1];

	int max_index;
	int idx;

	Server(int port, string pw): port(port), passwd(pw), clientaddr_len(sizeof(client_addr)) {}

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
				cout << "ip  : " << inet_ntoa(client_addr.sin_addr) << endl;
				cout << "port: " << ntohs(client_addr.sin_port) << endl;
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
						Client::taken.erase(client[i].nickname);
						client_map.erase(client[i].nickname);
						for (map<string, Channel>::iterator mitr = channel.begin(); mitr != channel.end(); ++mitr)
							mitr->second.member.erase(i);
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
					if (channel.find(*itr) == channel.end())
						channel[*itr] = Channel();
					channel[*itr].member.insert(i);
				}
		}
		else if (command == "PRIVMSG")
		{
			if (arg.size() > 1 && client_map.find(arg[0]) != client_map.end())
				send(client[client_map[arg[0]]].fd, client[i].msg.c_str(), client[i].msg.size(), 0);
		}


		client[i].msg = "";
	}
};

#endif