#ifndef CLIENT_HPP
# define CLIENT_HPP

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::map;
using std::set;
using std::vector;
using std::priority_queue;

class Client
{
public:
	set<string> joined_channel;
	bool is_authenticated;
	bool is_registered;
	string nickname;
	string username;
	string msg;
	int idx;
	int fd;

	Client() : is_authenticated(false), is_registered(false), nickname(""), username(""), msg(""), idx(-1), fd(-1) {}
	Client(int idx, int fd) : is_authenticated(false), is_registered(false), nickname(""), username(""), msg(""), idx(idx), fd(fd) {}
	Client(const Client& a)
	{
		*this = a;
	}
	Client& operator=(const Client& a)
	{
		is_authenticated = a.is_authenticated;
		is_registered = a.is_registered;
		nickname = a.nickname;
		username = a.username;
		msg = a.msg;
		idx = a.idx;
		fd = a.fd;
		return *this;
	}
	void feed(char *buf)
	{
		msg += buf;
	}
	void authenticate(string passwd, vector<string> arg)
	{
		if (arg.size() == 1 && passwd == arg[0])
		{
			sendMsg("authenticated\n");
			is_authenticated = true;
		}
		else
		{
			sendMsg("wrong pw\n");
		}
	}
	void nick(map<string, int> &client_map, vector<string> args)
	{
		if (args.size() != 1)
		{
			sendMsg("invalid num of args\n");
		}
		string arg = args[0];
		if (client_map.find(arg) != client_map.end())
		{
			sendMsg("already taken.\n");
			return ;
		}
		nickname = arg;
		client_map[arg] = idx;
		if (is_authenticated && nickname != "" && username != "")
		{
			is_registered = true;
			sendMsg("registered\n");
		}
	}
	void user(vector<string> args)
	{
		if (args.size() != 1)
		{
			sendMsg("invalid num of args\n");
			return ;
		}
		username = args[0];
		if (is_authenticated && nickname != "" && username != "")
		{
			is_registered = true;
			sendMsg("registered\n");
		}
	}

	void sendMsg(string message, int flag=0)
	{
		send(fd, message.c_str(), message.size(), flag);
	}
	string prefix(void)
	{
		return string(":") + username + ' ';
	}
};

#endif