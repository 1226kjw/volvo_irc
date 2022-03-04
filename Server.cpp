#include "Server.hpp"

vector<string> split(string str, char d=' ')
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

bool isin(char c, string pool)
{
	return (pool.find(c) != string::npos);
}

Server::Server(int port, string pw): port(port), passwd(pw), clientaddr_len(sizeof(client_addr)) {}
Server::~Server()
{
	for (int i = 0; i < CLIENT_MAX; ++i)
		if (client_fd[i].fd != -1)
			close(client_fd[i].fd);
}

int Server::setup()
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

int Server::run()
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
			continue ;
		}
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
				if (*--client[i].msg.end() == '\n')
					cmd(i);
			}
		}
	}
}

// client[i].fd == client_fd[i].fd
// i != client[i].fd
void Server::cmd(int i)
{
	cout << "from " << i << ':' << client[i].msg;
	vector<string> tok = split(client[i].msg);
	if (tok.back().back() == '\n')
		tok.back().pop_back();
	
	if (tok.size() == 0)
		return ;
	
	string command = tok[0];
	vector<string> arg(tok.begin() + 1, tok.end());

	try
	{
		if (!client[i].is_registered)
			enroll(i, command, arg);
		else if (command == "JOIN")
			join(i, arg);
		else if (command == "KICK")
			kick(i, arg);
		else if (command == "PART")
			;
		else if (command == "PRIVMSG" || command == "NOTICE")
			privmsg(i, arg);
		else if (command == "QUIT")
			quit(i, arg);
		else
			client[i].sendMsg("invalid command\n");
	}
	catch(const std::exception& e)
	{
		client[i].sendMsg(e.what());
	}
	client[i].msg = "";
}
void Server::enroll(int i, string command, vector<string> arg)
{
	if (!client[i].is_authenticated)
	{
		if (command == "PASS")
			client[i].authenticate(passwd, arg);
		else
			client[i].sendMsg("authenticate first\n");
	}
	else if (command == "PASS")
		client[i].sendMsg("already authenticated\n");
	else if (command == "NICK")
		client[i].nick(client_map, arg);
	else if (command == "USER")
		client[i].user(arg);
	else
		client[i].sendMsg("register first\n");
	client[i].msg = "";
}
void Server::join(int i, vector<string> arg)
{
	if (arg.size() == 1 && arg[0] == "0")
		while (!client[i].joined_channel.empty())
			channel[*client[i].joined_channel.begin()].out(client[i]);
	else
		for (vector<string>::iterator itr = arg.begin(); itr != arg.end(); ++itr)
		{
			if (isin(itr->front(), CHANNEL_PREFIX))
			{
				client[i].sendMsg("invalid arg\n");
				return ;
			}
			if (channel.find(*itr) == channel.end())
				channel[*itr] = Channel(*itr, client[i].nickname);
			channel[*itr].join(client[i]);
		}
}

void Server::list(int i, vector<string> arg)
{
	// if (arg.size() != 1)
}

void Server::kick(int i, vector<string> arg)
{
	if (arg.size() == 2)
		arg.push_back("");
	if (arg.size() != 3)
		throw std::invalid_argument("invalid num of args\n");

	if (isin(arg[0][0], CHANNEL_PREFIX))
		client[i].sendMsg("invalid arg\n");
	else if (channel.find(arg[0]) == channel.end())
		client[i].sendMsg("invalid channel\n");
	else if (channel[arg[0]].member.find(client_map[arg[1]]) == channel[arg[0]].member.end())
		client[i].sendMsg("invalid user\n");
	client[i].sendMsg(arg[2]);
	channel[arg[0]].out(client[i]);
}

void Server::privmsg(int i, vector<string> arg)
{
	if (arg.size() < 2)
		throw std::invalid_argument("invalid num of args\n");
	for (vector<string>::iterator itr = arg.begin(); itr != --arg.end(); ++itr)
	{
		if (isin(itr->front(), CHANNEL_PREFIX))
		{
			if (channel.find(*itr) != channel.end())
				channel[*itr].sendMsg(client, i, client[i].msg);
			else
				client[i].sendMsg(string("No such channel: ") + *itr + "\n");
		}
		else
		{
			if (client_map.find(*itr) != client_map.end())
				client[client_map[*itr]].sendMsg(client[i].prefix() + client[i].msg);
			else
				client[i].sendMsg(string("No such client: ") + *itr + "\n");
		}
	}
}
void Server::quit(int i, vector<string> arg)
{
	for (set<string>::iterator itr = client[i].joined_channel.begin(); itr != client[i].joined_channel.end(); ++itr)
	{
		vector<string> tmp = vector<string>(1, *itr);
		tmp.push_back(".");
		privmsg(i, tmp);
	}
	while (!client[i].joined_channel.empty())
		channel[*client[i].joined_channel.begin()].out(client[i]);
	cout << "byebye " << client[i].nickname << endl;
	close(client[i].fd);
	client_map.erase(client[i].nickname);
	client_fd[i].events = 0;
	client_fd[i].fd = -1;
	available_index.push(-i);
}
