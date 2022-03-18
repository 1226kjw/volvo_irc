#include "Server.hpp"

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::map;
using std::set;
using std::make_pair;
using std::vector;
using std::priority_queue;

Server::Server(int port, string pw): port(port), passwd(pw), clientaddr_len(sizeof(client_addr)), oper_name("kim"), oper_pw("lee") {}

Server::~Server()
{
	for (int i = 0; i < CLIENT_MAX; ++i)
		if (client_fd[i].fd != -1)
			close(client_fd[i].fd);
}

int Server::setup()
{
	if ((server_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
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
			idx = -available_index.top();
			available_index.pop();
			cout << "idx : " << idx << endl << endl;
			client_fd[idx].fd = client_socket;
			client_fd[idx].events = POLLIN;
			client[idx] = Client(idx, client_socket);
			if (available_index.size() == 0)
			{
				client[idx].sendMsg("Already full\n");
				close(client[idx].fd());
				available_index.push(-idx);
				continue;
			}
			if (idx == max_index)
				++max_index;
			cout << "ip  : " << inet_ntoa(client_addr.sin_addr) << endl;
			cout << "port: " << ntohs(client_addr.sin_port) << endl;
			cout << "client accepted on\nfd : " << client_socket << endl;
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
				cout << "good bye " << client[i].nickname() << endl;
				cout << "index " << i << " is available" << endl;
				client_map.erase(client[i].nickname());
				while (!client[i].joined_channel().empty())
					channel[*client[i].joined_channel().begin()].out(client[i], channel);
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
	vector<string> tok = split(client[i].message().substr(0, client[i].message().size() - 1));
	
	if (tok.size() == 0)
		return ;
	
	string command = tok[0];
	vector<string> arg(tok.begin() + 1, tok.end());

	try
	{
		if (!client[i].is_registered() && command != "PASS" && command != "NICK" && command != "USER")
			throw std::invalid_argument("register first\n");
		if (command == "PASS")
			pass(i, arg);
		else if (command == "NICK")
			nick(i, arg);
		else if (command == "USER")
			user(i, arg);
		else if (command == "OPER")
			oper(i, arg);
		else if (command == "MODE")
			mode(i, arg);
		else if (command == "JOIN")
			join(i, arg);
		else if (command == "KICK")
			kick(i, arg);
		else if (command == "PART")
			part(i, arg);
		else if (command == "NAMES")
			names(i, arg);
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

void Server::pass(int i, vector<string> arg)
{
	if (client[i].is_authenticated())
		throw ERR_ALREADY_AUTHENTICATED();
	if (arg.size() != 1)
		throw ERR_NEEDMOREPARAMS();
	client[i].authenticate(passwd, arg);
}

void Server::nick(int i, vector<string> arg)
{
	if (arg.size() != 1)
		throw ERR_NEEDMOREPARAMS();
	if (!nickcheck(arg[0])) 
		throw ERR_ERRONEUSNICKNAME();
	if (client_map.find(arg[0]) != client_map.end())
		throw ERR_NICKNAMEINUSE();
	client_map[arg[0]] = i;
	client[i].nick(client_map, arg[0]);
}

void Server::user(int i, vector<string> arg)
{
	if (client[i].is_registered())
		throw ERR_ALREADYREGISTRED();
	if (arg.size() != 4)
		throw ERR_NEEDMOREPARAMS();
	int mode;
	try
	{
		mode = irc_atoi(arg[1]);
	}
	catch(const std::exception& e)
	{
		mode = 0;
	}
	if (mode & MODE_o)
	{
		client[i].sendMsg("You are not operator\n");
		mode -= MODE_o;
	}
	client[i].mode_remove(0x7fffffff);
	client[i].mode_add(mode);
	client[i].user(arg);	
}

void Server::oper(int i, vector<string> arg)
{
	if (arg.size() != 2)
		throw ERR_NEEDMOREPARAMS();
	if (oper_name != arg[0])
		throw ERR_NOOPERHOST();
	if (oper_pw != arg[1])
		throw ERR_PASSWDMISMATCH();
	client[i].mode_add(MODE_o);
}

void Server::mode(int i, vector<string> arg)
{
	if (arg.size() != 2)
		throw ERR_NEEDMOREPARAMS();
	if (client_map.find(arg[0]) == client_map.end())
		throw ERR_NOSUCHUSER();
	if (arg[1].size() != 2 || !isin(arg[1][0], "+-") || !isin(arg[1][1], "io"))
		throw ERR_UMODEUNKNOWNFLAG();
	if (arg[1][0] == '-')
	{
		if (arg[1][1] == 'i')
		{
			if (client[i].nickname() != arg[0] && !(client[i].mode() & MODE_o))
				throw ERR_USERSDONTMATCH();
			client[client_map[arg[0]]].mode_remove(MODE_i);
		}
		else if (arg[1][1] == 'o')
		{
			if (client[i].nickname() != arg[0] && !(client[i].mode() & MODE_o))
				throw ERR_USERSDONTMATCH();
			client[client_map[arg[0]]].mode_remove(MODE_o);
		}
	}
	else if (arg[1][0] == '+')
	{
		if (arg[1][1] == 'i')
		{
			if (client[i].nickname() != arg[0] && !(client[i].mode() & MODE_o))
				throw ERR_USERSDONTMATCH();
			client[client_map[arg[0]]].mode_add(MODE_i);
		}
		else if (arg[1][1] == 'o')
		{
			if (client[i].nickname() == arg[0] || !(client[i].mode() & MODE_o))
				throw ERR_USERSDONTMATCH();
			client[client_map[arg[0]]].mode_add(MODE_o);
		}
	}
}

void Server::join(int i, vector<string> arg)
{
	if (arg.size() == 1 && arg[0] == "0")
		while (!client[i].joined_channel().empty())
			channel[*client[i].joined_channel().begin()].out(client[i], channel);
	else
		for (vector<string>::iterator itr = arg.begin(); itr != arg.end(); ++itr)
		{
			if (!isin(itr->at(0), CHANNEL_PREFIX))
				throw ERR_NOSUCHCHANNEL();
			if (channel.find(*itr) == channel.end())
				channel[*itr] = Channel(*itr);
			channel[*itr].join(client[i]);
		}
}

void Server::part(int i, vector<string> arg)
{
	if (arg.size() != 1)
		throw ERR_NEEDMOREPARAMS();
	if (channel.find(arg[0]) == channel.end())
		throw ERR_NOSUCHCHANNEL();
	channel[arg[0]].sendMsg(client, i, client[i].prefix() + client[i].message());
	channel[arg[0]].out(client[i], channel);
}

void Server::names(int i, vector<string> arg)
{
	if (arg.size() > 1)
		throw ERR_TOOMANYPARAMS();
	string buf;
	map<string, bool> visited;
	for (map<string, int>::iterator itr = client_map.begin(); itr != client_map.end(); ++itr)
		visited[itr->first] = false;
	if (arg.size() == 0)
	{
		for (map<string, Channel>::iterator itr = channel.begin(); itr != channel.end() ; ++itr)
		{
			buf += itr->first + ": { ";
			for (set<int>::iterator sitr = itr->second.member().begin(); sitr != itr->second.member().end(); ++sitr)
			{
				if (client[*sitr].mode() & MODE_i)
					continue;
				buf += client[*sitr].nickname() + ' ';
				visited[client[*sitr].nickname()] = true;
			}
			buf += "}\n";
		}
		buf += "*: { ";
		for (map<string, bool>::iterator itr = visited.begin(); itr != visited.end(); ++itr)
			if (!itr->second && !(client[client_map[itr->first]].mode() & MODE_i))
				buf += itr->first + ' ';
		buf += "}\n";

	}
	else
	{
		if (channel.find(arg[0]) == channel.end())
			throw ERR_NOSUCHCHANNEL();
		buf += arg[0] + ": { ";
		for (set<int>::iterator sitr = channel[arg[0]].member().begin(); sitr != channel[arg[0]].member().end(); ++sitr)
		{
			if (client[*sitr].mode() & MODE_i)
				continue;
			buf += client[*sitr].nickname() + ' ';
		}
		buf += "}\n";
	}

	client[i].sendMsg(buf);
}

void Server::kick(int i, vector<string> arg)
{
	if (arg.size() != 2)
		throw ERR_NEEDMOREPARAMS();
	if (!isin(arg[0][0], CHANNEL_PREFIX))
		throw ERR_NOSUCHCHANNEL();
	if (channel.find(arg[0]) == channel.end())
		throw ERR_NOSUCHCHANNEL();
	if (!(channel[arg[0]].isin(client_map[arg[1]])))
		throw ERR_USERNOTINCHANNEL();
	if (!(client[i].mode() & MODE_o))
		throw ERR_CHANOPRIVSNEEDED();
	channel[arg[0]].sendMsg(client, i, client[i].prefix() + client[i].message());
	channel[arg[0]].out(client[client_map[arg[1]]], channel);
}

void Server::privmsg(int i, vector<string> arg)
{
	if (arg.size() < 2)
		throw ERR_NEEDMOREPARAMS();
	for (vector<string>::iterator itr = arg.begin(); itr != --arg.end(); ++itr)
	{
		if (isin(itr->at(0), CHANNEL_PREFIX))
		{
			if (channel.find(*itr) == channel.end())
				throw ERR_CANNOTSENDTOCHAN();
			channel[*itr].sendMsg(client, i, client[i].prefix() + client[i].message());
		}
		else
		{
			if (client_map.find(*itr) == client_map.end())
				throw ERR_NOSUCHNICK();
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
		channel[*client[i].joined_channel().begin()].out(client[i], channel);
	cout << "good bye " << client[i].nickname() << endl;
	close(client[i].fd());
	client_map.erase(client[i].nickname());
	client_fd[i].events = 0;
	client_fd[i].fd = -1;
	available_index.push(-i);
}

const char* Server::ERR_ALREADY_AUTHENTICATED::what() const throw()
{
	return "Already authenticated\n";
}

const char* Server::ERR_NEEDMOREPARAMS::what() const throw()
{
	return "Need more parameters\n";
}

const char* Server::ERR_ERRONEUSNICKNAME::what() const throw()
{
	return "Invalid Nickname\n";
}

const char* Server::ERR_ALREADYREGISTRED::what() const throw()
{
	return "Already Registered\n";
}

const char* Server::ERR_NOSUCHCHANNEL::what() const throw()
{
	return "No such channel\n";
}

const char* Server::ERR_USERNOTINCHANNEL::what() const throw()
{
	return "There is no user in the channel\n";
}

const char* Server::ERR_CHANOPRIVSNEEDED::what() const throw()
{
	return "You are not operator\n";
}

const char* Server::ERR_NOSUCHNICK::what() const throw()
{
	return "No such nickname\n";
}

const char* Server::ERR_NICKNAMEINUSE::what() const throw()
{
	return "Nickname already used\n";
}

const char* Server::ERR_NOOPERHOST::what() const throw()
{
	return "Wrong host name\n";
}

const char* Server::ERR_PASSWDMISMATCH::what() const throw()
{
	return "Wrong host password\n";
}

const char* Server::ERR_USERSDONTMATCH::what() const throw()
{
	return "User don't match\n";
}

const char* Server::ERR_UMODEUNKNOWNFLAG::what() const throw()
{
	return "Wrong mode\n";
}

const char *Server::ERR_CANNOTSENDTOCHAN::what() const throw()
{
	return "Cannot send to channel\n";
}

const char *Server::ERR_NOSUCHUSER::what() const throw()
{
	return "No such user\n";
}

const char* Server::ERR_TOOMANYPARAMS::what() const throw()
{
	return "There are too many parameters\n";
}
