#include "Server.hpp"

const char* Server::ERR_WRONG_PW::what() const throw()
{
	return "Wrong password\n";
}

const char* Server::ERR_NICKNAMEINUSE::what() const throw()
{
	return "Nickname already taken\n";
}

const char* Server::ERR_NEEDMOREPARAMS::what() const throw()
{
	return "Need more parameters\n";
}

const char* Server::ERR_UNAUTHENTICATED::what() const throw()
{
	return "Unauthenticated\n";
}

const char* Server::ERR_ALREADY_AUTHENTICATED::what() const throw()
{
	return "Already authenticated\n";
}

const char* Server::ERR_NOT_REGISTERED::what() const throw()
{
	return "Not registered\n";
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
			cout << "ip  : " << inet_ntoa(client_addr.sin_addr) << endl;
			cout << "port: " << ntohs(client_addr.sin_port) << endl;
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
				close(client[i].fd());
				cout << "byebye " << client[i].nickname() << endl;
				cout << "index " << i << " is available" << endl;
				client_map.erase(client[i].nickname());
				while (!client[i].joined_channel().empty())
					channel[*client[i].joined_channel().begin()].out(client[i]);
				client_fd[i].events = 0;
				client_fd[i].fd = -1;
				available_index.push(-i);
			}
			else
			{
				buf[recv_size] = 0;
				client[i].feed(buf);
				if (*--client[i].message().end() == '\n')
					cmd(i);
			}
		}
	}
}

void Server::cmd(int i)
{
	if (client[i].message() == "\n")
	{
		client[i].message("");
		return ;
	}
	cout << "from " << i << ':' << client[i].message();
	vector<string> tok = split(client[i].message());
	if (tok.back().back() == '\n')
		tok.back().pop_back();
	
	if (tok.size() == 0)
		return ;
	
	string command = tok[0];
	vector<string> arg(tok.begin() + 1, tok.end());

	try
	{
		if (!client[i].is_registered())
			enroll(i, command, arg);
		else if (command == "JOIN")
			join(i, arg);
		else if (command == "KICK")
			kick(i, arg);
		else if (command == "PART")
			part(i, arg);
		else if (command == "PRIVMSG" || command == "NOTICE")
			privmsg(i, arg);
		else if (command == "QUIT")
			quit(i);
		else
			client[i].sendMsg("invalid command\n");
	}
	catch(const std::exception& e)
	{
		client[i].sendMsg(e.what());
	}
	client[i].message("");
}

void Server::enroll(int i, string command, vector<string> arg)
{
	int state;
	if (!client[i].is_authenticated())
	{
		if (command == "PASS")
			state = client[i].authenticate(passwd, arg);
		else
			state = UNAUTHENTICATED;
	}
	else if (command == "PASS")
		state = ALREADY_AUTHENTICATED;
	else if (command == "NICK")
		state = client[i].nick(client_map, arg);
	else if (command == "USER")
		state = client[i].user(arg);
	else
		state = NOT_REGISTERED;

	switch (state)
	{
	case VALID:
		break;
	case WRONG_PW:
		throw ERR_WRONG_PW();
	case NICKNAMEINUSE:
		throw ERR_NICKNAMEINUSE();
	case NEEDMOREPARAMS:
		throw ERR_NEEDMOREPARAMS();
	case UNAUTHENTICATED:
		throw ERR_UNAUTHENTICATED();
	case ALREADY_AUTHENTICATED:
		throw ERR_ALREADY_AUTHENTICATED();
	case NOT_REGISTERED:
		throw ERR_NOT_REGISTERED();
	default:
		cerr << "unknown error" << endl;
		break;
	}
	client[i].message("");
}

void Server::join(int i, vector<string> arg)
{
	if (arg.size() == 1 && arg[0] == "0")
		while (!client[i].joined_channel().empty())
			channel[*client[i].joined_channel().begin()].out(client[i]);
	else
		for (vector<string>::iterator itr = arg.begin(); itr != arg.end(); ++itr)
		{
			if (!isin(itr->front(), CHANNEL_PREFIX))
				throw std::invalid_argument("invalid arg\n");
			if (channel.find(*itr) == channel.end())
				channel[*itr] = Channel(*itr, client[i].nickname());
			channel[*itr].join(client[i]);
		}
}

void Server::part(int i, vector<string> arg)
{
	if (arg.size() != 1)
		throw std::invalid_argument("invalid num of args\n");
	if (channel.find(arg[0]) == channel.end())
		throw std::invalid_argument("no such channel\n");
	channel[arg[0]].sendMsg(client, i, client[i].prefix() + client[i].message());
	channel[arg[0]].out(client[i]);
}

void Server::kick(int i, vector<string> arg)
{
	if (arg.size() != 2)
		throw std::invalid_argument("invalid num of args\n");
	if (!isin(arg[0][0], CHANNEL_PREFIX))
		throw std::invalid_argument("invalid arg\n");
	if (channel.find(arg[0]) == channel.end())
		throw std::invalid_argument("invalid channel\n");
	if (channel[arg[0]].isin(client_map[arg[1]]))
		throw std::invalid_argument("invalid user\n");
	if (channel[arg[0]].manager() != client[i].username())
		throw std::invalid_argument("not authorized\n");
	channel[arg[0]].sendMsg(client, i, client[i].prefix() + client[i].message());
	channel[arg[0]].out(client[client_map[arg[1]]]);
}

void Server::privmsg(int i, vector<string> arg)
{
	if (arg.size() < 2)
		throw std::invalid_argument("invalid num of args\n");
	for (vector<string>::iterator itr = arg.begin(); itr != --arg.end(); ++itr)
	{
		if (isin(itr->front(), CHANNEL_PREFIX))
		{
			if (channel.find(*itr) == channel.end())
				throw std::invalid_argument("no such channel\n");
			channel[*itr].sendMsg(client, i, client[i].prefix() + client[i].message());
		}
		else
		{
			if (client_map.find(*itr) == client_map.end())
				throw std::invalid_argument("no such user\n");
			client[client_map[*itr]].sendMsg(client[i].prefix() + client[i].message());
		}
	}
}
void Server::quit(int i)
{
	for (set<string>::iterator itr = client[i].joined_channel().begin(); itr != client[i].joined_channel().end(); ++itr)
	{
		vector<string> tmp = vector<string>(1, *itr);
		tmp.push_back(".");
		privmsg(i, tmp);
	}
	while (!client[i].joined_channel().empty())
		channel[*client[i].joined_channel().begin()].out(client[i]);
	cout << "byebye " << client[i].nickname() << endl;
	close(client[i].fd());
	client_map.erase(client[i].nickname());
	client_fd[i].events = 0;
	client_fd[i].fd = -1;
	available_index.push(-i);
}
